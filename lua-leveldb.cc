#include <iostream>

// leveldb
#include <leveldb/db.h>

// leveldb atomic operation library
#include <leveldb/write_batch.h>

#define LUALEVELDB_VERSION		"lua-leveldb 0.2.0"
#define LUALEVELDB_COPYRIGHT	"Copyright (C) 2012, lua-leveldb by Marco Pompili (marcs.pompili@gmail.com)."
#define LUALEVELDB_DESCRIPTION	"Bindings for Google's leveldb library."

#define LUALEVELDB_FN			"leveldb"
#define LUALEVELDB_DB_MT		"leveldb.database"
#define LUALEVELDB_ITER_MT		"leveldb.iterator"
#define LUALEVELDB_BATCH_MT		"leveldb.batch"

using namespace std;
using namespace leveldb;

extern "C" {

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static int lvldb_f_open(lua_State *L) {
	const char *filename = luaL_checkstring(L, 1);

	DB *db;
	Options options;

	//TODO Make these options dynamic, eventually...
	options.create_if_missing = true;
	options.error_if_exists = false;

	Status s = leveldb::DB::Open(options, filename, &db);

	if (!s.ok())
		cerr << "LvlDB_f_open: Error opening creating database: " << s.ToString() << endl;
	else {
		// Pushing pointer to the database in the stack
		lua_pushlightuserdata(L, db);

		luaL_getmetatable(L, LUALEVELDB_DB_MT);
		lua_setmetatable(L, -2);
	}

	return 1;
}

static int lvldb_f_close(lua_State *L) {
	DB *db = (leveldb::DB*) lua_touserdata(L, 1);

	cout << "LvlDB_f_close: Closing DB!\n";

	delete db;

	return 0;
}

static int lvldb_f_check(lua_State *L) {
	DB *db = (leveldb::DB*) lua_touserdata(L, 1);

	cout << "LvlDB_f_check: checking database instance.\n";
	lua_pushboolean(L, db != NULL ? true : false);

	return 1;
}

// service function for checking user-data
static leveldb::DB *check_leveldb(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, LUALEVELDB_DB_MT);
	luaL_argcheck(L, ud != NULL, 1, "'database' expected");

	return (DB *)ud;
}

static int lvldb_db_put(lua_State *L) {
	DB *db = check_leveldb(L);

	string key = (const char*) lua_tostring(L, 2);
	string value = (const char*) lua_tostring(L, 3);

	Status s = db->Put(leveldb::WriteOptions(), key, value);

	if (s.ok()) {
		lua_pushboolean(L, true);
	} else {
		cerr << "Error putting value key: " << key << " with value " << value << endl;
	}

	return 1;
}

static int lvldb_db_del(lua_State *L) {
	DB *db = check_leveldb(L);

	string key = (const char *) luaL_checkstring(L, 2);

	Status s = db->Delete(leveldb::WriteOptions(), key);

	return 1;
}

static int lvldb_db_get(lua_State *L) {
	DB *db = check_leveldb(L);

	string key = (const char*) luaL_checkstring(L, 2);
	string value;

	Status s = db->Get(leveldb::ReadOptions(), key, &value);

	if (s.ok()) {
		lua_pushstring(L, value.c_str());
	} else {
		cerr << "Error getting value with key: " << key << endl;
		lua_pushboolean(L, false);
	}

	return 1;
}

static int lvldb_db_iterator(lua_State *L) {
	DB *db = check_leveldb(L);

	Iterator *it = db->NewIterator(leveldb::ReadOptions());

	lua_pushlightuserdata(L, it);

	return 1;
}

// TODO: test this method
static int lvldb_db_batch(lua_State *L) {
	DB *db = check_leveldb(L);

	if(db != NULL) {
		WriteBatch *batch;
		lua_pushlightuserdata(L, batch);
	}

	return 1;
}

static int lvldb_db_snapshot(lua_State *L) {
	DB *db = check_leveldb(L);

	leveldb::ReadOptions options;
	options.snapshot = db->GetSnapshot();

	leveldb::Iterator *iter = db->NewIterator(options);
	delete iter;

	db->ReleaseSnapshot(options.snapshot);

	return 1;
}

static WriteBatch *check_batch(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, LUALEVELDB_BATCH_MT);
	luaL_argcheck(L, ud != NULL, 1, "'batch' expected");

	return (WriteBatch *) ud;
}

static int lvldb_batch_put(lua_State *L) {
	WriteBatch *batch = check_batch(L);

	const char *key = luaL_checkstring(L, 2);
	const char *value = luaL_checkstring(L, 3);

	batch->Put(key, value);

	return 0;
}

static int lvldb_batch_del(lua_State *L) {
	WriteBatch *batch = check_batch(L);

	const char *key = luaL_checkstring(L, 2);

	batch->Delete(key);

	return 0;
}

static Iterator *check_it(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, LUALEVELDB_ITER_MT);
	luaL_argcheck(L, ud != NULL, 1, "'iterator' expected");

	return (Iterator *) ud;
}

static int lvldb_it_valid(lua_State *L) {
	Iterator *it = check_it(L);

	lua_pushboolean(L, it->Valid());

	return 1;
}

static int lvldb_it_next(lua_State *L) {
	Iterator *it = check_it(L);

	it->Next();

	return 0;
}

// empty
static const struct luaL_reg E [] = { { NULL, NULL } };

// static oriented functions
static const struct luaL_reg leveldb_f [] = {
	{ "open", lvldb_f_open },
	{ "check", lvldb_f_check },
	{ "close", lvldb_f_close },
	{ NULL, NULL }
};

// object oriented methods for database object
static const struct luaL_reg leveldb_m_db [] = {
	{ "put", lvldb_db_put },
	{ "delete", lvldb_db_del },
	{ "get", lvldb_db_get },
	{ "iterator", lvldb_db_iterator },
	{ "batch", lvldb_db_batch },
	{ "snapshot", lvldb_db_snapshot },
	{ NULL, NULL }
};

static const struct luaL_reg leveldb_m_btch [] = {
	{ "put",  lvldb_batch_put },
	{ "delete", lvldb_batch_del },
	{ NULL, NULL }
};

// object oriented methods for iterator
static const struct luaL_reg leveldb_m_iter [] = {
	{ "valid", lvldb_it_valid },
	{ "next" , lvldb_it_next },
	{ NULL, NULL }
};

// stack procedure for adding a library
static void add_lib(lua_State *L, const char *metatable, const struct luaL_reg lib []) {

	// let's build the function meta-table
	luaL_newmetatable(L, metatable);

	// meta-table already in the stack
	lua_pushstring(L, "__index");

	lua_pushvalue(L, -2); // push the meta-table
	lua_settable(L, -3); // meta-table.__index = meta-table

	// function table already on the stack
	luaL_openlib(L, NULL, lib, 0);
}

int luaopen_leveldb(lua_State *L) {

	// register module
	luaL_register(L, "leveldb", E);

	// register module information
	lua_pushliteral(L, LUALEVELDB_VERSION);
	lua_setfield(L, -2, "_VERSION");

	lua_pushliteral(L, LUALEVELDB_COPYRIGHT);
	lua_setfield(L, -2, "_COPYRIGHT");

	lua_pushliteral(L, LUALEVELDB_DESCRIPTION);
	lua_setfield(L, -2, "_DESCRIPTION");

	// adding database, iterator, batch methods
	add_lib(L, LUALEVELDB_DB_MT, leveldb_m_db);
	add_lib(L, LUALEVELDB_ITER_MT, leveldb_m_iter);
	add_lib(L, LUALEVELDB_BATCH_MT, leveldb_m_btch);

	// static functions registered on library
	luaL_openlib(L, LUALEVELDB_FN, leveldb_f, 0);

	return 1;
}

}
