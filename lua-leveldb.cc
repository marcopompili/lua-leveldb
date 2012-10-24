#include <iostream>

// LevelDB CPP main library
#include <leveldb/db.h>

// LevelDB atomic operation library
#include <leveldb/write_batch.h>

#define LUALEVELDB_VERSION		"lua-leveldb 0.1.2"
#define LUALEVELDB_COPYRIGHT	"Copyright (C) 2012, lua-leveldb by Marco Pompili (marcs.pompili@gmail.com)."
#define LUALEVELDB_DESCRIPTION	"Bindings for Google's leveldb library."

#define LUALEVELDB_DB_MT		"google.leveldb"
#define LUALEVELDB_DB_NM		"leveldb"

extern "C" {

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static int LvlDB_f_open(lua_State *L) {
	const char *filename = luaL_checkstring(L, 1);

	leveldb::DB *db;
	leveldb::Options options;

	//TODO Make these options dynamic, eventually...
	options.create_if_missing = true;
	options.error_if_exists = false;

	leveldb::Status s = leveldb::DB::Open(options, filename, &db);

	if (!s.ok())
		std::cerr << "LvlDB_f_open: Error opening creating database: " << s.ToString() << std::endl;
	else {
		// Pushing pointer to the database in the stack
		lua_pushlightuserdata(L, db);

		luaL_getmetatable(L, LUALEVELDB_DB_MT);
		lua_setmetatable(L, -2);
	}

	return 1;
}

static int LvlDB_f_close(lua_State *L) {
	leveldb::DB *db = (leveldb::DB*) lua_touserdata(L, 1);

	std::cout << "LvlDB_f_close: Closing DB!\n";

	delete db;

	return 0;
}

static int LvlDB_f_check(lua_State *L) {
	leveldb::DB *db = (leveldb::DB*) lua_touserdata(L, 1);

	std::cout << "LvlDB_f_check: checking database instance.\n";
	lua_pushboolean(L, db != NULL ? true : false);

	return 1;
}

// service function for checking user-data
static leveldb::DB *check_leveldb(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, LUALEVELDB_DB_MT);
	luaL_argcheck(L, ud != NULL, 1, "'leveldb' expected");

	return (leveldb::DB *)ud;
}

static int LvlDB_m_put(lua_State *L) {
	leveldb::DB *db = check_leveldb(L);

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
	leveldb::DB *db = check_leveldb(L);

	std::string key = (const char*) luaL_checkstring(L, 2);
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
	leveldb::DB *db = check_leveldb(L);

	std::string key = (const char *) luaL_checkstring(L, 2);

	leveldb::Status s = db->Delete(leveldb::WriteOptions(), key);

	return 1;
}

// TODO: check this method and link it to the method meta-table
static int LvlDB_m_batch(lua_State *L) {
	leveldb::DB *db = check_leveldb(L);

	std::string key = luaL_checkstring(L, 1);
	std::string value = luaL_checkstring(L, 2);

	leveldb::Status s = db->Get(leveldb::ReadOptions(), key, &value);

	if (s.ok()) {
		leveldb::WriteBatch batch;
	}

	return 1;
}

static int LvlDB_m_snapshot(lua_State *L) {
	leveldb::DB *db = check_leveldb(L);

	leveldb::ReadOptions options;
	options.snapshot = db->GetSnapshot();

	leveldb::Iterator *iter = db->NewIterator(options);
	delete iter;

	db->ReleaseSnapshot(options.snapshot);

	return 1;
}

// static oriented functions
static const struct luaL_reg leveldb_f [] = {
		{ "open", LvlDB_f_open },
		{ "check", LvlDB_f_check},
		{ "close", LvlDB_f_close},
		{ NULL, NULL }
};

// object oriented methods
static const struct luaL_reg leveldb_m [] = {
		{ "put", LvlDB_m_put },
		{ "get", LvlDB_m_get },
		{ "delete", LvlDB_m_del },
		{ "batch", LvlDB_m_batch },
		{ "snapshot", LvlDB_m_snapshot },
		{ NULL, NULL }
};

int luaopen_leveldb(lua_State *L) {

	luaL_newmetatable(L, LUALEVELDB_DB_MT);

	// meta-table already in the stack
	lua_pushstring(L, "__index");

	lua_pushvalue(L, -2); // push the meta-table
	lua_settable(L, -3); // meta-table.__index = meta-table

	// package table on the stack
	luaL_openlib(L, NULL, leveldb_m, 0);

	// static functions registered on library
	luaL_openlib(L, LUALEVELDB_DB_NM, leveldb_f, 0);

	return 1;
}

}
