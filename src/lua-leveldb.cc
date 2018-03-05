#include "lua-leveldb.hpp"

// Rock info
#define LUALEVELDB_VERSION	"Lua-LevelDB 0.4.0"
#define LUALEVELDB_COPYRIGHT	"Copyright (C) 2012-18, Lua-LevelDB by Marco Pompili (pompilimrc@gmail.com)."
#define LUALEVELDB_DESCRIPTION	"Lua bindings for Google's LevelDB library."
#define LUALEVELDB_LOGMODE	0

using namespace std;
using namespace leveldb;

/**
 * Opens a DB connection, based on the given options and filename.
 */
int lvldb_open(lua_State *L) {
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

/**
 * Close an open DB instance.
 */
int lvldb_close(lua_State *L) {
  DB *db = (DB*) lua_touserdata(L, 1);

  delete db;

  return 0;
}

/**
 * Create an options object with the defaults values.
 */
int lvldb_options(lua_State *L) {
  Options *optp = (Options*)lua_newuserdata(L, sizeof(Options));

  *(optp) = Options(); // set default values

  luaL_getmetatable(L, LVLDB_MT_OPT);
  lua_setmetatable(L, -2);

  return 1;
}

/**
 * Create a WriteBatch object.
 */
int lvldb_batch(lua_State *L) {
  WriteBatch batch; // initialization
  WriteBatch *batchp = &batch; // store pointer

  lua_pushlightuserdata(L, batchp);
  luaL_getmetatable(L, LVLDB_MT_BATCH);
  lua_setmetatable(L, -2);

  return 1;
}

/**
 * Check for DB basic consistency.
 */
int lvldb_check(lua_State *L) {
  DB *db = (DB*) lua_touserdata(L, 1);

  lua_pushboolean(L, db != NULL ? true : false);

  return 1;
}

/**
 * Try repair the DB with the name in input.
 * -----------------------------------------
 * From the LevelDB documentation:
 * If a database is corrupted (perhaps it cannot be opened when
 * paranoid checking is turned on), the leveldb::RepairDB function may
 * be used to recover as much of the data as possible.
 */
int lvldb_repair(lua_State *L) {
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


int lvldb_bloom_filter_policy(lua_State *L) {
  luaL_checktype(L, 1, LUA_TUSERDATA);
  Options *opt = (Options*)luaL_checkudata(L, 1, LVLDB_MT_OPT);
  int bits_per_key = lua_tonumber(L, 2);
  const FilterPolicy *fp = NewBloomFilterPolicy(bits_per_key);
  
  opt->filter_policy = fp;

  return 0;
}


extern "C" {

  // Initialization
  LUALIB_API int luaopen_liblualeveldb(lua_State *L) {
    lua_newtable(L);

    // register module information
    lua_pushliteral(L, LUALEVELDB_VERSION);
    lua_setfield(L, -2, "_VERSION");

    lua_pushliteral(L, LUALEVELDB_COPYRIGHT);
    lua_setfield(L, -2, "_COPYRIGHT");

    lua_pushliteral(L, LUALEVELDB_DESCRIPTION);
    lua_setfield(L, -2, "_DESCRIPTION");

    // LevelDB functions
#if LUA_VERSION_NUM > 501
    luaL_setfuncs(L, lvldb_leveldb_m, 0);
#else
    luaL_register(L, NULL, lvldb_leveldb_m);
#endif

    // initialize meta-tables methods
    init_metatable(L, LVLDB_MT_DB, lvldb_database_m);
    init_complex_metatable(L, LVLDB_MT_OPT, lvldb_options_m, lvldb_options_meta, options_getters, options_setters);
    init_complex_metatable(L, LVLDB_MT_ROPT, lvldb_read_options_m, lvldb_read_options_meta, read_options_getters, read_options_setters);
    init_complex_metatable(L, LVLDB_MT_WOPT, lvldb_write_options_m, lvldb_write_options_meta, write_options_getters, write_options_setters);
    init_metatable(L, LVLDB_MT_ITER, lvldb_iterator_m);
    init_metatable(L, LVLDB_MT_BATCH, lvldb_batch_m);

    return 1;
  }

}
