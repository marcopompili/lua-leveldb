#include <iostream>

// leveldb
#include <leveldb/db.h>

// leveldb atomic operation library
#include <leveldb/write_batch.h>

#define LUALEVELDB_VERSION			"lua-leveldb 0.2.0"
#define LUALEVELDB_COPYRIGHT		"Copyright (C) 2012, lua-leveldb by Marco Pompili (marcs.pompili@gmail.com)."
#define LUALEVELDB_DESCRIPTION		"Bindings for Google's leveldb library."

#define LVLDB_MOD_NAME		"leveldb"
#define LVLDB_MT_OPT		"leveldb.opt"
#define LVLDB_MT_DB			"leveldb.db"
#define LVLDB_MT_BATCH		"leveldb.btch"

extern "C" {

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

}

static int get_int(lua_State *L, void *v) {
	lua_pushnumber(L, *(int*) v);
	return 1;
}

static int set_int(lua_State *L, void *v) {
	*(int*) v = luaL_checkint(L, 3);
	return 0;
}

static int get_number(lua_State *L, void *v) {
	lua_pushnumber(L, *(lua_Number*) v);
	return 1;
}

static int set_number(lua_State *L, void *v) {
	*(lua_Number*) v = luaL_checknumber(L, 3);
	return 0;
}

static int get_bool(lua_State *L, void *v) {
	lua_pushboolean(L, *(bool*) v);
	return 1;
}

static int set_bool(lua_State *L, void *v) {
	*(bool*) v = lua_toboolean(L, 3);
	return 0;
}

static int get_string(lua_State *L, void *v) {
	lua_pushstring(L, (char*) v);
	return 1;
}

typedef int (*Xet_func)(lua_State *L, void *v);

/* member info for get and set handlers */
typedef const struct {
	const char *name; /* member name */
	Xet_func func; /* get or set function for type of member */
	size_t offset; /* offset of member within your_t */
} Xet_reg_pre;

typedef Xet_reg_pre * Xet_reg;

using namespace std;

static void Xet_add(lua_State *L, Xet_reg l) {
	for (; l->name; l++) {
		lua_pushstring(L, l->name);
		lua_pushlightuserdata(L, (void*) l);
		lua_settable(L, -3);
	}
}

static int Xet_call(lua_State *L) {
	// for get: stack has userdata, index, lightuserdata
	// for set: stack has userdata, index, value, lightuserdata
	Xet_reg m = (Xet_reg) lua_touserdata(L, -1);
	lua_pop(L, 1);

	// drop lightuserdata
	luaL_checktype(L, 1, LUA_TUSERDATA);
	return m->func(L, (void *) ((char *) lua_touserdata(L, 1) + m->offset));
}

static int index_handler(lua_State *L) {
	// stack has userdata, index
	lua_pushvalue(L, 2); // dup index
	lua_rawget(L, lua_upvalueindex(1)); // lookup member by name

	if (!lua_islightuserdata(L, -1)) {
		lua_pop(L, 1);
		// drop value
		lua_pushvalue(L, 2); // dup index
		lua_gettable(L, lua_upvalueindex(2)); // else try methods

		if (lua_isnil(L, -1)) // invalid member
			luaL_error(L, "cannot get member '%s'", lua_tostring(L, 2));

		return 1;
	}

	return Xet_call(L); // call get function
}

static int newindex_handler(lua_State *L) {
	// stack has userdata, index, value
	lua_pushvalue(L, 2); // dup index
	lua_rawget(L, lua_upvalueindex(1)); // lookup member by name

	if (!lua_islightuserdata(L, -1)) // invalid member
		luaL_error(L, "cannot set member '%s'", lua_tostring(L, 2));

	return Xet_call(L); // call set function
}

static void init_complex_metatable(lua_State *L, const char *metatable_name, const luaL_reg methods[], const luaL_reg metamethods[], const Xet_reg_pre getters[], const Xet_reg_pre setters[]) {

	// create methods table, & add it to the table of globals
	luaL_openlib(L, metatable_name, methods, 0);
	int methods_stack = lua_gettop(L);

	// create metatable for object, & add it to the registry
	luaL_newmetatable(L, metatable_name);
	luaL_openlib(L, 0, metamethods, 0); // fill metatable
	int metatable_stack = lua_gettop(L);

	lua_pushliteral(L, "__metatable");
	lua_pushvalue(L, methods_stack); // dup methods table
	lua_rawset(L, metatable_stack); // hide metatable

	lua_pushliteral(L, "__index");
	lua_pushvalue(L, metatable_stack);
	Xet_add(L, getters);
	lua_pushvalue(L, methods_stack);
	lua_pushcclosure(L, index_handler, 2);
	lua_rawset(L, metatable_stack);

	lua_pushliteral(L, "__newindex");
	lua_newtable(L);
	Xet_add(L, setters);
	lua_pushcclosure(L, newindex_handler, 1);
	lua_rawset(L, metatable_stack);

	lua_pop(L, 1);
}

// procedure for adding a metatable into the stack
static void init_metatable(lua_State *L, const char *metatable, const struct luaL_reg lib[]) {

	// let's build the function metatable
	if (luaL_newmetatable(L, metatable) == 0)
		cerr << "Warning: metatable " << metatable << " is already set" << endl;

	lua_pushstring(L, "__index"); // metatable already in the stack
	lua_pushvalue(L, -2); // push the metatable
	lua_settable(L, -3); // metatable.__index = metatable

	// metatable already on the stack
	luaL_openlib(L, NULL, lib, 0);
}

using namespace leveldb;

static DB *check_database(lua_State *L, int index) {
	void *ud = luaL_checkudata(L, 1, LVLDB_MT_DB);
	luaL_argcheck(L, ud != NULL, 1, "'database' expected");

	return (DB *) ud;
}

static Options *check_options(lua_State *L, int index) {
	Options *opt;
	luaL_checktype(L, 1, LUA_TUSERDATA);
	opt = (Options*)luaL_checkudata(L, index, LVLDB_MT_OPT);

	if(opt == NULL)
		luaL_typerror(L, index, LVLDB_MT_OPT);

	return opt;
}

static WriteBatch *check_writebatch(lua_State *L, int index) {
	void *ud = luaL_checkudata(L, 1, LVLDB_MT_BATCH);
	luaL_argcheck(L, ud != NULL, 1, "'batch' expected");

	return (WriteBatch *) ud;
}

static int lvldb_open(lua_State *L) {
	DB *db;
	Options *opt = check_options(L, 1);
	const char *filename = luaL_checkstring(L, 2);

	cout << "Pointer: " << opt << endl;
	cout << "Address: " << &opt << endl;

	cout << "CreateIfMissing: " << opt->create_if_missing << endl;

	Status s = DB::Open(*(opt), filename, &db);

	if (!s.ok())
		cerr << "lvldb_open: Error opening creating database: " << s.ToString() << endl;
		else {
			// Pushing pointer to the database in the stack
			lua_pushlightuserdata(L, db);

			luaL_getmetatable(L, LVLDB_MT_DB);
			lua_setmetatable(L, -2);
		}

	return 1;
}

static int lvldb_close(lua_State *L) {
	DB *db = (DB*) lua_touserdata(L, 1);

	cout << "lvldb_close: Closing DB!\n";

	delete db;

	return 0;
}

static int lvldb_options(lua_State *L) {
	(Options*)lua_newuserdata(L, sizeof(Options));
	luaL_getmetatable(L, LVLDB_MT_OPT);
	lua_setmetatable(L, -2);

	return 1;
}

static int lvldb_batch(lua_State *L) {
	(WriteBatch*)lua_newuserdata(L, sizeof(WriteBatch));
	luaL_getmetatable(L, LVLDB_MT_BATCH);
	lua_setmetatable(L, -2);

	return 1;
}

static int lvldb_check(lua_State *L) {
	DB *db = (DB*) lua_touserdata(L, 1);

	cout << "lvldb_check: checking database instance.\n";
	lua_pushboolean(L, db != NULL ? true : false);

	return 1;
}

static int lvldb_database_put(lua_State *L) {
	DB *db = check_database(L, 1);

	string key = (const char*) lua_tostring(L, 2);
	string value = (const char*) lua_tostring(L, 3);

	Status s = db->Put(WriteOptions(), key, value);

	if (s.ok()) {
		lua_pushboolean(L, true);
	} else {
		cerr << "Error putting value key: " << key << " with value " << value << endl;
	}

	return 1;
}

static int lvldb_database_get(lua_State *L) {
	DB *db = check_database(L, 1);

	string key = (const char*) luaL_checkstring(L, 2);
	string value;

	Status s = db->Get(ReadOptions(), key, &value);

	if (s.ok()) {
		lua_pushstring(L, value.c_str());
	} else {
		cerr << "Error getting value with key: " << key << endl;
		lua_pushboolean(L, false);
	}

	return 1;
}

static int lvldb_batch_put(lua_State *L) {
	WriteBatch batch = *(check_writebatch(L, 1));
	const char *key = luaL_checkstring(L, 2);
	const char *value = luaL_checkstring(L, 3);

	batch.Put(key, value);

	return 0;
}

// empty
static const struct luaL_reg E[] = { { NULL, NULL } };

static const luaL_reg lvldb_leveldb_m[] = {
		{ "open", lvldb_open },
		{ "close", lvldb_close },
		{ "options", lvldb_options },
		{ "batch", lvldb_batch },
		{ "check", lvldb_check },
		{ 0, 0 }
};

static const luaL_reg lvldb_options_m[] = {
		{ "create", NULL },
		{ 0, 0 }
};

static const luaL_reg lvldb_options_meta[] = {
		{ "__string", NULL },
		{ 0, 0}
};

// options getters
static const Xet_reg_pre options_getters[] = {
		{ "createIfMissing", get_bool, offsetof(Options, create_if_missing) },
		{ "errorIfExists", get_bool, offsetof(Options, error_if_exists) },
		{ "paranoidChecks", get_bool, offsetof(Options, paranoid_checks) },
		{ "writeBufferSize", get_int, offsetof(Options, write_buffer_size) },
		{ 0, 0 }
};

// options setters
static const Xet_reg_pre options_setters[] = {
		{ "createIfMissing", set_bool, offsetof(Options, create_if_missing) },
		{ "errorIfExists", set_bool, offsetof(Options, error_if_exists) },
		{ "paranoidChecks", set_bool, offsetof(Options, paranoid_checks) },
		{ "writeBufferSize", set_int, offsetof(Options, write_buffer_size) },
		{ 0, 0 }
};

static const luaL_reg lvldb_database_m[] = {
		{ "put", lvldb_database_put },
		{ "get", lvldb_database_get },
		{ 0, 0 }
};

static const luaL_reg lvldb_batch_m[] = {
		{ "put", lvldb_batch_put },
		{ 0, 0 }
};

extern "C" {

LUALIB_API int luaopen_leveldb(lua_State *L) {
	// register module
	luaL_register(L, LVLDB_MOD_NAME, E);

	// register module information
	lua_pushliteral(L, LUALEVELDB_VERSION);
	lua_setfield(L, -2, "_VERSION");

	lua_pushliteral(L, LUALEVELDB_COPYRIGHT);
	lua_setfield(L, -2, "_COPYRIGHT");

	lua_pushliteral(L, LUALEVELDB_DESCRIPTION);
	lua_setfield(L, -2, "_DESCRIPTION");

	// leveldb methods
	luaL_openlib(L, LVLDB_MOD_NAME, lvldb_leveldb_m, 0);

	init_metatable(L, LVLDB_MT_DB, lvldb_database_m);
	init_complex_metatable(L, LVLDB_MT_OPT, lvldb_options_m, lvldb_options_meta, options_getters, options_setters);
	init_metatable(L, LVLDB_MT_BATCH, lvldb_batch_m);

	return 0;
}

}
