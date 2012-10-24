#include <iostream>
#include <leveldb/db.h>

extern "C" {

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define LUALEVELDB_DB_MT		"leveldb.database"

using namespace std;

// service function for checking user-data
static leveldb::DB *check_leveldb(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, LUALEVELDB_DB_MT);
	luaL_argcheck(L, ud != NULL, 1, "'database' expected");

	return (leveldb::DB *)ud;
}

static int lvldb_db_put(lua_State *L) {
	leveldb::DB *db = check_leveldb(L);

	string key = (const char*) lua_tostring(L, 2);
	string value = (const char*) lua_tostring(L, 3);

	leveldb::Status s = db->Put(leveldb::WriteOptions(), key, value);

	if (s.ok()) {
		lua_pushboolean(L, true);
	} else {
		cerr << "Error putting value key: " << key << " with value " << value << endl;
	}

	return 1;
}

static int lvldb_db_del(lua_State *L) {
	leveldb::DB *db = check_leveldb(L);

	string key = (const char *) luaL_checkstring(L, 2);

	leveldb::Status s = db->Delete(leveldb::WriteOptions(), key);

	return 1;
}

static int lvldb_db_get(lua_State *L) {
	leveldb::DB *db = check_leveldb(L);

	string key = (const char*) luaL_checkstring(L, 2);
	string value;

	leveldb::Status s = db->Get(leveldb::ReadOptions(), key, &value);

	if (s.ok()) {
		lua_pushstring(L, value.c_str());
	} else {
		cerr << "Error getting value with key: " << key << endl;
		lua_pushboolean(L, false);
	}

	return 1;
}

static int lvldb_db_iterator(lua_State *L) {
	leveldb::DB *db = check_leveldb(L);

	leveldb::Iterator *it = db->NewIterator(leveldb::ReadOptions());

	lua_pushlightuserdata(L, it);

	return 1;
}

// TODO: check this method and link it to the method meta-table
static int lvldb_db_batch(lua_State *L) {
	leveldb::DB *db = check_leveldb(L);

	string key = luaL_checkstring(L, 1);
	string value = luaL_checkstring(L, 2);

	//TODO implement

	return 1;
}

static int lvldb_db_snapshot(lua_State *L) {
	leveldb::DB *db = check_leveldb(L);

	leveldb::ReadOptions options;
	options.snapshot = db->GetSnapshot();

	leveldb::Iterator *iter = db->NewIterator(options);
	delete iter;

	db->ReleaseSnapshot(options.snapshot);

	return 1;
}

// object oriented methods for db object
static const struct luaL_reg leveldb_db [] = {
		{ "put", lvldb_db_put },
		{ "delete", lvldb_db_del },
		{ "get", lvldb_db_get },
		{ "iterator", lvldb_db_iterator },
		{ "batch", lvldb_db_batch },
		{ "snapshot", lvldb_db_snapshot },
		{ NULL, NULL }
};

}
