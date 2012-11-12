#include <iostream>
#include <sstream>

// leveldb
#include <leveldb/db.h>
#include <leveldb/status.h>
#include <leveldb/options.h>

// leveldb atomic operation library
#include <leveldb/write_batch.h>

#include "config.h"

#define LUALEVELDB_VERSION		"lua-leveldb 0.2.0"
#define LUALEVELDB_COPYRIGHT		"Copyright (C) 2012, lua-leveldb by Marco Pompili (marcs.pompili@gmail.com)."
#define LUALEVELDB_DESCRIPTION		"Bindings for Google's leveldb library."
#define LUALEVELDB_LOGMODE			0

#define LVLDB_MOD_NAME			"leveldb"
#define LVLDB_MT_OPT			"leveldb.opt"
#define LVLDB_MT_ROPT			"leveldb.ropt"
#define LVLDB_MT_WOPT			"leveldb.wopt"
#define LVLDB_MT_DB			"leveldb.db"
#define LVLDB_MT_ITER			"leveldb.iter"
#define LVLDB_MT_BATCH			"leveldb.btch"

extern "C" {

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

}

/**
 * Basic setters and getters
 */
static int get_int(lua_State *L, void *v) {
	lua_pushnumber(L, *(int*) v);
	return 1;
}

static int set_int(lua_State *L, void *v) {
	*(int*) v = luaL_checkint(L, 3);
	return 0;
}

/*
static int get_number(lua_State *L, void *v) {
	lua_pushnumber(L, *(lua_Number*) v);
	return 1;
}

static int set_number(lua_State *L, void *v) {
	*(lua_Number*) v = luaL_checknumber(L, 3);
	return 0;
}
*/

static int get_size(lua_State *L, void *v) {
	lua_pushinteger(L, *(lua_Integer*) v);
	return 1;
}

static int set_size(lua_State *L, void *v) {
	*(lua_Integer*) v = luaL_checkinteger(L, 3);
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

/*
//TODO test it
static int get_string(lua_State *L, void *v) {
	lua_pushstring(L, (char*) v);
	return 1;
}

//TODO test it
static int set_string(lua_State *L, void *v) {
	v = (char*)lua_tostring(L, 3);
	return 0;
}
*/

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

static Slice lua_param_to_slice(lua_State *L, int i) {
	if(lua_isnumber(L, i)) {
		lua_Number lua_number = lua_tonumber(L, i);
		char *lua_number_str;
		lua_number2str(lua_number_str, lua_number);
		return Slice((const char*) lua_number_str);
	}
	else if(lua_isstring(L, i))
		return Slice(lua_tostring(L, i));
	else {
		return NULL;
	}
}

static string bool_to_string(int boolean) {
	return boolean == 1 ? "true" : "false";
}

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

static ReadOptions *check_read_options(lua_State *L, int index) {
	void *ud = luaL_checkudata(L, index, LVLDB_MT_ROPT);
	luaL_argcheck(L, ud != NULL, index, "'writeOptions' expected");

	return (ReadOptions *) ud;
}

static WriteOptions *check_write_options(lua_State *L, int index) {
	void *ud = luaL_checkudata(L, index, LVLDB_MT_WOPT);
	luaL_argcheck(L, ud != NULL, index, "'readOptions' expected");

	return (WriteOptions *) ud;
}

static Iterator *check_iter(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, LVLDB_MT_ITER);
	luaL_argcheck(L, ud != NULL, 1, "'iterator' expected");

	return (Iterator *) ud;
}

static WriteBatch *check_writebatch(lua_State *L, int index) {
	void *ud = luaL_checkudata(L, 1, LVLDB_MT_BATCH);
	luaL_argcheck(L, ud != NULL, 1, "'batch' expected");

	return (WriteBatch *) ud;
}

#define lvldb_opt(L, l) ( lua_gettop(L) >= l ? *(check_options(L, l)) : Options() )
#define lvldb_ropt(L, l) ( lua_gettop(L) >= l ? *(check_read_options(L, l)) : ReadOptions() )
#define lvldb_wopt(L, l) ( lua_gettop(L) >= l ? *(check_write_options(L, l)) : WriteOptions() )

static int lvldb_open(lua_State *L) {
	DB *db;
	Options *opt = check_options(L, 1);
	const char *filename = luaL_checkstring(L, 2);

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

	delete db;

	return 0;
}

static int lvldb_options(lua_State *L) {
	Options *optp = (Options*)lua_newuserdata(L, sizeof(Options));

	*(optp) = Options(); // set default values

	luaL_getmetatable(L, LVLDB_MT_OPT);
	lua_setmetatable(L, -2);

	return 1;
}

static int lvldb_options_tostring(lua_State *L) {
	Options *opt = check_options(L, 1);

	ostringstream oss (ostringstream::out);
	oss << "Create if missing: " << bool_to_string(opt->create_if_missing) << endl;
	oss << "Error if exists: " << bool_to_string(opt->error_if_exists) << endl;
	oss << "Paranoid checks: " << bool_to_string(opt->paranoid_checks) << endl;
	oss << "Environment: " << opt->env << endl;
	oss << "Info log: " << opt->info_log << endl;
	oss << "Write buffer size: " << opt->write_buffer_size << endl;
	oss << "Max open files: " << opt->max_open_files << endl;
	oss << "Block cache: " << opt->block_cache << endl;
	oss << "Block size: " << opt->block_size << endl;
	oss << "Block restart interval: " << opt->block_restart_interval << endl;

	string compression = opt->compression == 1 ? "kSnappyCompression" : "None";
	oss << "Compression: " << compression << endl;

	#ifdef LEVELDB_FILTER_POLICY_H
		oss << "Filter policy: " << opt->filter_policy << endl;
	#endif

	lua_pushstring(L, oss.str().c_str());

	return 1;
}

static int lvldb_read_options(lua_State *L) {
	ReadOptions *ropt = (ReadOptions*)lua_newuserdata(L, sizeof(ReadOptions));

	*(ropt) = ReadOptions(); // set default values

	luaL_getmetatable(L, LVLDB_MT_ROPT);
	lua_setmetatable(L, -2);
	return 1;
}

static int lvldb_read_options_tostring(lua_State *L) {
	ReadOptions *ropt = check_read_options(L, 1);

	ostringstream oss (ostringstream::out);
	oss << "Verify checksum: " << bool_to_string(ropt->verify_checksums) << endl;
	oss << "Fill cache: " << bool_to_string(ropt->fill_cache) << endl;
	oss << "Snapshot: " << ropt->snapshot << endl;

	lua_pushstring(L, oss.str().c_str());

	return 1;
}

static int lvldb_write_options(lua_State *L) {
	WriteOptions *wopt = (WriteOptions*)lua_newuserdata(L, sizeof(WriteOptions));

	*(wopt) = WriteOptions(); // set default values

	luaL_getmetatable(L, LVLDB_MT_WOPT);
	lua_setmetatable(L, -2);
	return 1;
}

static int lvldb_write_options_tostring(lua_State *L) {
	WriteOptions *wopt = check_write_options(L, 1);

	ostringstream oss (ostringstream::out);
	oss << "Sync: " << bool_to_string(wopt->sync) << endl;

	lua_pushstring(L, oss.str().c_str());

	return 1;
}

static int lvldb_batch(lua_State *L) {
	WriteBatch batch; // initialization
	WriteBatch *batchp = &batch; // store pointer

	lua_pushlightuserdata(L, batchp);
	luaL_getmetatable(L, LVLDB_MT_BATCH);
	lua_setmetatable(L, -2);

	return 1;
}

static int lvldb_check(lua_State *L) {
	DB *db = (DB*) lua_touserdata(L, 1);

	lua_pushboolean(L, db != NULL ? true : false);

	return 1;
}

static int lvldb_repair(lua_State *L) {
	string dbname = luaL_checkstring(L, 1);

	Status s = leveldb::RepairDB(dbname, lvldb_opt(L, 2));

	if(s.ok())
		lua_pushboolean(L, true);
	else {
		cerr << "Error repairing database: " << s.ToString() << endl;
		lua_pushboolean(L, false);
	}

	return 1;
}

static int lvldb_database_put(lua_State *L) {
	DB *db = check_database(L, 1);

	Slice key = Slice(lua_param_to_slice(L, 2));
	Slice value = Slice(lua_param_to_slice(L, 3));

	Status s = db->Put(lvldb_wopt(L ,4), key, value);

	if (s.ok())
		lua_pushboolean(L, true);
	else {
		cerr << "Error inserting key/value: " << s.ToString() << endl;
		lua_pushboolean(L, false);
	}

	return 1;
}

static int lvldb_database_get(lua_State *L) {
	DB *db = check_database(L, 1);

	Slice key = Slice(lua_param_to_slice(L, 2));
	string value;

	Status s = db->Get(lvldb_ropt(L, 3), key, &value);

	if (s.ok())
		lua_pushstring(L, value.c_str());
	else {
		cerr << "Error getting value: " << s.ToString() << endl;
		lua_pushboolean(L, false);
	}

	return 1;
}

static int lvldb_database_set(lua_State *L) {
	DB *db = check_database(L, 1);
	string value = luaL_checkstring(L, 2);
	unsigned double i = 0;
	
	Iterator *it = db->NewIterator(lvldb_ropt(L, 3));

	for (it->SeekToFirst(); it->Valid(); it->Next()) {
		cout << i;
		if (value != it->value().ToString()) {
			char *id_str;
			Slice key = Slice(lua_number2str(id_str, i));
			Status s = db->Put(WriteOptions(), key, value);

			assert(s.ok());
		}
		i++;
	}

	assert(it->status().ok());

	delete it;

	return 0;
}

static int lvldb_database_del(lua_State *L) {
	DB *db = check_database(L, 1);

	Slice key = Slice(lua_param_to_slice(L, 2));

	Status s = db->Delete(lvldb_wopt(L, 3), key);

	if (s.ok())
		lua_pushboolean(L, true);
	else {
		cerr << "Error deleting key/value entry: " << s.ToString() << endl;
		lua_pushboolean(L, false);
	}

	return 1;
}

static int lvldb_database_iterator(lua_State *L) {
	DB *db = check_database(L, 1);

	Iterator *it = db->NewIterator(lvldb_ropt(L, 2));
	lua_pushlightuserdata(L, it);

	luaL_getmetatable(L, LVLDB_MT_ITER);
	lua_setmetatable(L, -2);

	return 1;
}

//TODO needs testing
static int lvldb_database_write(lua_State *L) {
	DB *db = check_database(L, 1);

	WriteBatch batch = *(check_writebatch(L, 2));

	Status s = db->Write(lvldb_wopt(L, 3), &batch);

	return 0;
}

static int lvldb_database_snapshot(lua_State *L) {
	DB *db = check_database(L, 1);

	const Snapshot *snapshot = db->GetSnapshot();

	lua_pushlightuserdata(L, (void*)snapshot);

	return 1;
}

static int lvldb_database_tostring(lua_State *L) {
	DB *db = check_database(L, 1);
	ostringstream oss (ostringstream::out);

	Iterator* it = db->NewIterator(ReadOptions());

	for (it->SeekToFirst(); it->Valid(); it->Next()) {
		oss << it->key().ToString() << " -> "  << it->value().ToString() << endl;
		#ifdef LUALEVELDB_LOGMODE
			cout << "LOG: " << it->key().ToString() << " -> "  << it->value().ToString() << endl;
		#endif
	}
	
	assert(it->status().ok());
	
	delete it;
	
	lua_pushstring(L, oss.str().c_str());
	
	return 1;
}

static int lvldb_iterator_seek(lua_State *L) {
	Iterator *iter = check_iter(L);

	string start = luaL_checkstring(L, 2);

	iter->Seek(start);

	return 0;
}

static int lvldb_iterator_seek_to_first(lua_State *L) {
	Iterator *iter = check_iter(L);

	iter->SeekToFirst();

	return 0;
}

static int lvldb_iterator_seek_to_last(lua_State *L) {
	Iterator *iter = check_iter(L);

	iter->SeekToLast();

	return 0;
}

static int lvldb_iterator_valid(lua_State *L) {
	Iterator *iter = check_iter(L);

	lua_pushboolean(L, iter->Valid());

	return 1;
}

static int lvldb_iterator_next(lua_State *L) {
	Iterator *iter = check_iter(L);

	iter->Next();

	return 0;
}

static int lvldb_iterator_key(lua_State *L) {
	Iterator *iter = check_iter(L);

	lua_pushstring(L, iter->key().ToString().c_str());

	return 1;
}

static int lvldb_iterator_value(lua_State *L) {
	Iterator *iter = check_iter(L);

	lua_pushstring(L, iter->value().ToString().c_str());

	return 1;
}

static int lvldb_batch_tostring(lua_State *L) {
	WriteBatch batch = *(check_writebatch(L, 1));

	ostringstream oss (ostringstream::out);
	oss << "Batch" << endl;
	lua_pushstring(L, oss.str().c_str());

	return 1;
}

static int lvldb_batch_put(lua_State *L) {
	WriteBatch batch = *(check_writebatch(L, 1));

	Slice key = Slice(lua_param_to_slice(L, 2));
	Slice value = Slice(lua_param_to_slice(L, 3));

	batch.Put(key, value);

	return 0;
}

static int lvldb_batch_del(lua_State *L) {
	WriteBatch batch = *(check_writebatch(L, 1));

	Slice key = Slice(lua_param_to_slice(L, 2));

	batch.Delete(key);

	return 0;
}

static int lvldb_batch_clear(lua_State *L) {
	WriteBatch batch = *(check_writebatch(L, 1));

	batch.Clear();

	return 0;
}

// empty
static const struct luaL_reg E[] = { { NULL, NULL } };

// main methods
static const luaL_reg lvldb_leveldb_m[] = {
		{ "open", lvldb_open },
		{ "close", lvldb_close },
		{ "options", lvldb_options },
		{ "readOptions", lvldb_read_options },
		{ "writeOptions", lvldb_write_options },
		{ "repair ", lvldb_repair },
		{ "batch", lvldb_batch },
		{ "check", lvldb_check },
		{ 0, 0 }
};

// options methods
static const luaL_reg lvldb_options_m[] = {
		{ 0, 0 }
};

// options metamethods
static const luaL_reg lvldb_options_meta[] = {
		{ "__tostring", lvldb_options_tostring },
		{ 0, 0}
};

// options getters
static const Xet_reg_pre options_getters[] = {
		{ "createIfMissing", get_bool, offsetof(Options, create_if_missing) },
		{ "errorIfExists", get_bool, offsetof(Options, error_if_exists) },
		{ "paranoidChecks", get_bool, offsetof(Options, paranoid_checks) },
		{ "writeBufferSize", get_size, offsetof(Options, write_buffer_size) },
		{ "maxOpenFiles", get_int, offsetof(Options, max_open_files) },
		{ "blockSize", get_size, offsetof(Options, block_size) },
		{ "blockRestartInterval", get_int, offsetof(Options, block_restart_interval) },
		{ 0, 0 }
};

// options setters
static const Xet_reg_pre options_setters[] = {
		{ "createIfMissing", set_bool, offsetof(Options, create_if_missing) },
		{ "errorIfExists", set_bool, offsetof(Options, error_if_exists) },
		{ "paranoidChecks", set_bool, offsetof(Options, paranoid_checks) },
		{ "writeBufferSize", set_size, offsetof(Options, write_buffer_size) },
		{ "maxOpenFiles", set_int, offsetof(Options, max_open_files) },
		{ "blockSize", set_size, offsetof(Options, block_size) },
		{ "blockRestartInterval", set_int, offsetof(Options, block_restart_interval) },
		{ 0, 0 }
};

// read options methods
static const luaL_reg lvldb_read_options_m[] = {
		{ 0, 0 }
};

// read options metamethods
static const luaL_reg lvldb_read_options_meta[] = {
		{ "__tostring", lvldb_read_options_tostring },
		{ 0, 0 }
};

// read options getters
static const Xet_reg_pre read_options_getters[] = {
		{ "verifyChecksum", get_bool, offsetof(ReadOptions, verify_checksums) },
		{ "fillCache", get_bool, offsetof(ReadOptions, fill_cache) },
		{ 0, 0 }
};

// read options setters
static const Xet_reg_pre read_options_setters[] = {
		{ "verifyChecksum", set_bool, offsetof(ReadOptions, verify_checksums) },
		{ "fillCache", set_bool, offsetof(ReadOptions, fill_cache) },
		{ 0, 0 }
};

// write options methods
static const luaL_reg lvldb_write_options_m[] = {
		{ 0, 0 }
};

// write options metamethods
static const luaL_reg lvldb_write_options_meta[] = {
		{ "__tostring", lvldb_write_options_tostring },
		{ 0, 0 }
};

// write options getters
static const Xet_reg_pre write_options_getters[] = {
		{ "sync", get_bool, offsetof(WriteOptions, sync) },
		{ 0, 0 }
};

// write options setters
static const Xet_reg_pre write_options_setters[] = {
		{ "sync", set_bool, offsetof(WriteOptions, sync) },
		{ 0, 0 }
};

// database methods
static const luaL_reg lvldb_database_m[] = {
		{ "__tostring", lvldb_database_tostring },
		{ "put", lvldb_database_put },
		{ "set", lvldb_database_set },
		{ "get", lvldb_database_get },
		{ "delete", lvldb_database_del },
		{ "iterator", lvldb_database_iterator },
		{ "write", lvldb_database_write },
		{ "snapshot", lvldb_database_snapshot },
		{ 0, 0 }
};

// iterator methods
static const struct luaL_reg lvldb_iterator_m[] = {
		{ "seek", lvldb_iterator_seek },
		{ "seekToFirst", lvldb_iterator_seek_to_first },
		{ "seekToLast", lvldb_iterator_seek_to_last },
		{ "valid", lvldb_iterator_valid },
		{ "next", lvldb_iterator_next },
		{ "key", lvldb_iterator_key },
		{ "value", lvldb_iterator_value },
		{ NULL, NULL }
};

// batch methods
static const luaL_reg lvldb_batch_m[] = {
		{ "__tostring", lvldb_batch_tostring },
		{ "put", lvldb_batch_put },
		{ "delete", lvldb_batch_del },
		{ "clear", lvldb_batch_clear },
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

	// initialize metatables
	init_metatable(L, LVLDB_MT_DB, lvldb_database_m);
	init_complex_metatable(L, LVLDB_MT_OPT, lvldb_options_m, lvldb_options_meta, options_getters, options_setters);
	init_complex_metatable(L, LVLDB_MT_ROPT, lvldb_read_options_m, lvldb_read_options_meta, read_options_getters, read_options_setters);
	init_complex_metatable(L, LVLDB_MT_WOPT, lvldb_write_options_m, lvldb_write_options_meta, write_options_getters, write_options_setters);
	init_metatable(L, LVLDB_MT_ITER, lvldb_iterator_m);
	init_metatable(L, LVLDB_MT_BATCH, lvldb_batch_m);

	return 0;
}

}
