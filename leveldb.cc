#include <iostream>
#include <leveldb/db.h>

//#include "leveldb_db.h"
//#include "leveldb_batch.h"
//#include "leveldb_iterator.h"

#define LUALEVELDB_VERSION		"lua-leveldb 0.2.0"
#define LUALEVELDB_COPYRIGHT	"Copyright (C) 2012, lua-leveldb by Marco Pompili (marcs.pompili@gmail.com)."
#define LUALEVELDB_DESCRIPTION	"Bindings for Google's leveldb library."

#define LUALEVELDB_MT			"leveldb.main"

using namespace std;

extern "C" {

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static int lvldb_f_open(lua_State *L) {
	const char *filename = luaL_checkstring(L, 1);

	leveldb::DB *db;
	leveldb::Options options;

	//TODO Make these options dynamic, eventually...
	options.create_if_missing = true;
	options.error_if_exists = false;

	leveldb::Status s = leveldb::DB::Open(options, filename, &db);

	if (!s.ok())
		cerr << "LvlDB_f_open: Error opening creating database: " << s.ToString() << endl;
	else {
		// Pushing pointer to the database in the stack
		lua_pushlightuserdata(L, db);

		luaL_getmetatable(L, LUALEVELDB_MT);
		lua_setmetatable(L, -2);
	}

	return 1;
}

static int lvldb_f_close(lua_State *L) {
	leveldb::DB *db = (leveldb::DB*) lua_touserdata(L, 1);

	cout << "LvlDB_f_close: Closing DB!\n";

	delete db;

	return 0;
}

static int lvldb_f_check(lua_State *L) {
	leveldb::DB *db = (leveldb::DB*) lua_touserdata(L, 1);

	cout << "LvlDB_f_check: checking database instance.\n";
	lua_pushboolean(L, db != NULL ? true : false);

	return 1;
}

// empty
static const struct luaL_reg E [] = { { NULL, NULL } };

// static oriented functions
static const struct luaL_reg leveldb_f [] = {
		{ "open", lvldb_f_open },
		{ "check", lvldb_f_check},
		{ "close", lvldb_f_close},
		{ NULL, NULL }
};

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

	// let's build the function meta-table
	luaL_newmetatable(L, LUALEVELDB_MT);

	// meta-table already in the stack
	lua_pushstring(L, "__index");

	lua_pushvalue(L, -2); // push the meta-table
	lua_settable(L, -3); // meta-table.__index = meta-table

	// package table on the stack
	// luaL_openlib(L, NULL, leveldb_db, 0);

	// static functions registered on library
	luaL_openlib(L, "main", leveldb_f, 0);

	return 1;
}

}
