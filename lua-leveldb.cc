#include <iostream>

// LevelDB CPP main library
#include <leveldb/db.h>

// LevelDB atomic operation library
#include <leveldb/write_batch.h>

#define LUALEVELDB_VERSION		"lua-LevelDB 0.1.2"
#define LUALEVELDB_COPYRIGHT	"Copyright (C) 2012, lua-LevelDB by Marco Pompili (marcs.pompili@gmail.com)."
#define LUALEVELDB_DESCRIPTION	"Bindings for Google's LevelDB library."

#define LUALEVELDB_DB_MT		"leveldb.database"

extern "C" {

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static int LvlDB_f_open(lua_State *L) {
	const char* filename = luaL_checkstring(L, 1);
	leveldb::DB* db;

	leveldb::Options options;

	//TODO Make the options dynamic, eventually...
	options.create_if_missing = true;
	options.error_if_exists = false;

	leveldb::Status s = leveldb::DB::Open(options, filename, &db);

	if (!s.ok())
		std::cerr << "LvlDB_f_open: Error opening creating database: " << s.ToString() << std::endl;
	else
		(leveldb::DB*) lua_newuserdata(L, sizeof(leveldb::DB*));

	return 1;
}

static int LvlDB_f_close(lua_State *L) {
	leveldb::DB* db = (leveldb::DB*) lua_touserdata(L, 1);

	std::cout << "LvlDB_f_close: Closing DB!";

	delete db;

	return 0;
}

static int LvlDB_f_check(lua_State *L) {
	leveldb::DB* db = (leveldb::DB*) lua_touserdata(L, 1);

	std::cout << "LvlDB_f_check: checking database instance.";
	lua_pushboolean(L, db != NULL ? true : false);

	return 1;
}

static int LvlDB_m_put(lua_State *L) {
	leveldb::DB* db = (leveldb::DB*) lua_touserdata(L, 1);

	std::string key = (const char*) lua_tostring(L, 2);
	std::string value = (const char*) lua_tostring(L, 3);

	leveldb::Status s = db->Put(leveldb::WriteOptions(), key, value);

	if (s.ok()) {
		lua_pushboolean(L, true);
	} else {
		std::cerr << "Error putting value key: " << key << " with value " << value << std::endl;
	}

	return 1;
}

static int LvlDB_m_get(lua_State *L) {
	leveldb::DB* db = (leveldb::DB*) lua_touserdata(L, 1);

	std::string key = (const char*) lua_tostring(L, 2);
	std::string value;

	leveldb::Status s = db->Get(leveldb::ReadOptions(), key, &value);

	if (s.ok()) {
		lua_pushstring(L, value.c_str());
	} else {
		std::cerr << "Error getting value with key: " << key << std::endl;
		lua_pushboolean(L, false);
	}


	return 1;
}

static int LvlDB_m_del(lua_State *L) {
	leveldb::DB* db = (leveldb::DB*) lua_touserdata(L, 1);

	std::string key = (const char*) luaL_checkstring(L, 2);

	leveldb::Status s = db->Delete(leveldb::WriteOptions(), key);

	return 1;
}

// TODO: check this method and link it to the method meta-table
static int LvlDB_m_batch(lua_State *L) {
	leveldb::DB* db = (leveldb::DB*) lua_touserdata(L, 1);

	std::string key = luaL_checkstring(L, 1);
	std::string value = luaL_checkstring(L, 2);

	leveldb::Status s = db->Get(leveldb::ReadOptions(), key, &value);

	if (s.ok()) {
		leveldb::WriteBatch batch;
	}

	return 1;
}

static int LvlDB_m_snapshot(lua_State *L) {
	leveldb::DB* db = (leveldb::DB*) lua_touserdata(L, 1);

	leveldb::ReadOptions options;
	options.snapshot = db->GetSnapshot();

	leveldb::Iterator* iter = db->NewIterator(options);
	delete iter;
	db->ReleaseSnapshot(options.snapshot);

	return 1;
}

// static functions
static const struct luaL_reg leveldb_f [] = {
		{ "open", LvlDB_f_open },
		{ "check", LvlDB_f_check},
		{ "close", LvlDB_f_close},
		{ NULL, NULL }
};

// object methods
static const struct luaL_reg leveldb_m [] = {
		{ "put", LvlDB_m_put },
		{ "get", LvlDB_m_get },
		{ "delete", LvlDB_m_del },
		{ "snapshot", LvlDB_m_snapshot },
		{ NULL, NULL }
};

LUALIB_API int luaopen_leveldb(lua_State *L) {

	luaL_newmetatable(L, LUALEVELDB_DB_MT);

	// meta-table already in the stack
	lua_pushstring(L, "__index");

	lua_pushvalue(L, -2); // push the meta-table
	lua_settable(L, -3); // meta-table.__index = meta-table

	// package table on the stack
	luaL_openlib(L, NULL, leveldb_m, 0);

	// static functions registered on library
	luaL_openlib(L, "database", leveldb_f, 0);

	return 1;
}

}
