extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
}

#include <iostream>
#include <leveldb/db.h>

#define LUALEVELDB_VERSION     "lua-leveldb 0.1"
#define LUALEVELDB_COPYRIGHT   "Copyright (C) 2012, lua-leveldb Marco Pompili"
#define LUALEVELDB_DESCRIPTION "Bindings for Google's leveldb library"

leveldb::DB* db = NULL;

static int version(lua_State *L) {
	std::cout << LUALEVELDB_VERSION << std::endl;

	return 0;
}

extern "C" {

static int open(lua_State *L) {
	const char* filename = lua_tostring(L, 1);

	leveldb::Options options;
	options.create_if_missing = true;

	leveldb::Status s = leveldb::DB::Open(options, filename, &db);

	if (!s.ok()) {
		std::cerr << s.ToString() << std::endl;
	}

	return 0;
}

static int close(lua_State *L) {
	delete db;

	return 1;
}

static int get(lua_State *L) {
	std::string key = (const char*) lua_tostring(L, 1);
	std::string value;

	leveldb::Status s = db->Get(leveldb::ReadOptions(), key, &value);

	//if(s.ok())
	//lua_pushstring(L, (const char*)value, value.length());

	return 1;
}

static int put(lua_State *L) {
	std::string key = (const char*) lua_tostring(L, 2);
	std::string value = (const char*) lua_tostring(L, 3);

	leveldb::Status s = db->Get(leveldb::ReadOptions(), key, &value);
	return 1;
}

static int batch(lua_State *L) {
	std::string key = lua_tostring(L, 1);
	std::string value = lua_tostring(L, 2);
	leveldb::Status s = db->Get(leveldb::ReadOptions(), key, &value);

	if (s.ok()) {
		//leveldb::WriteBatch batch;
	}

	return 1;
}

static int check(lua_State *L) {
	if (db != NULL)
		lua_pushboolean(L, true);
	else
		lua_pushboolean(L, false);

	return 1;
}

static int snapshot(lua_State *L) {
	leveldb::DB* db = (leveldb::DB*) lua_touserdata(L, 1);
	leveldb::ReadOptions options;
	options.snapshot = db->GetSnapshot();

	leveldb::Iterator* iter = db->NewIterator(options);
	delete iter;
	db->ReleaseSnapshot(options.snapshot);

	return 1;
}

LUALIB_API int luaopen_leveldb(lua_State *L) {
	lua_register(L, "version", version);
	lua_register(L, "open", open);
	lua_register(L, "close", close);
	lua_register(L, "get", get);
	lua_register(L, "put", put);
	lua_register(L, "batch", batch);
	lua_register(L, "check", check);
	lua_register(L, "snapshot", snapshot);

	return 0;
}
}
