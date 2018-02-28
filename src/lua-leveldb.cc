#include "lua-leveldb.hpp"

// Rock info
#define LUALEVELDB_VERSION	"Lua-LevelDB 0.4.0"
#define LUALEVELDB_COPYRIGHT	"Copyright (C) 2012-18, Lua-LevelDB by Marco Pompili (pompilimrc@gmail.com)."
#define LUALEVELDB_DESCRIPTION	"Lua bindings for Google's LevelDB library."
#define LUALEVELDB_LOGMODE	0

using namespace std;
using namespace leveldb;

/**
 * Basic calls to LevelDB
 * ----------------------
 */

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

/**
 * Wrapping up the library into Lua
 * --------------------------------
 */
  
// empty
static const struct luaL_Reg E[] = { { NULL, NULL } };

// main methods
static const luaL_Reg lvldb_leveldb_m[] = {
  { "open", lvldb_open },
  { "close", lvldb_close },
  { "options", lvldb_options },
  { "readOptions", lvldb_read_options },
  { "writeOptions", lvldb_write_options },
  { "repair ", lvldb_repair },
  { "batch", lvldb_batch },
  { "check", lvldb_check },
  { "bloomFilterPolicy", lvldb_bloom_filter_policy },
  { NULL, NULL }
};

// options methods
static const luaL_Reg lvldb_options_m[] = {
  { NULL, NULL }
};

// options meta-methods
static const luaL_Reg lvldb_options_meta[] = {
  { "__tostring", lvldb_options_tostring },
  { NULL, NULL }
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
  { NULL, NULL }
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
  { NULL, NULL }
};

// read options methods
static const luaL_Reg lvldb_read_options_m[] = {
  { NULL, NULL }
};

// read options meta-methods
static const luaL_Reg lvldb_read_options_meta[] = {
  { "__tostring", lvldb_read_options_tostring },
  { NULL, NULL }
};

// read options getters
static const Xet_reg_pre read_options_getters[] = {
  { "verifyChecksum", get_bool, offsetof(ReadOptions, verify_checksums) },
  { "fillCache", get_bool, offsetof(ReadOptions, fill_cache) },
  { NULL, NULL }
};

// read options setters
static const Xet_reg_pre read_options_setters[] = {
  { "verifyChecksum", set_bool, offsetof(ReadOptions, verify_checksums) },
  { "fillCache", set_bool, offsetof(ReadOptions, fill_cache) },
  { NULL, NULL }
};

// write options methods
static const luaL_Reg lvldb_write_options_m[] = {
  { NULL, NULL }
};

// write options meta-methods
static const luaL_Reg lvldb_write_options_meta[] = {
  { "__tostring", lvldb_write_options_tostring },
  { NULL, NULL }
};

// write options getters
static const Xet_reg_pre write_options_getters[] = {
  { "sync", get_bool, offsetof(WriteOptions, sync) },
  { NULL, NULL }
};

// write options setters
static const Xet_reg_pre write_options_setters[] = {
  { "sync", set_bool, offsetof(WriteOptions, sync) },
  { NULL, NULL }
};

// database methods
static const luaL_Reg lvldb_database_m[] = {
  { "__tostring", lvldb_database_tostring },
  { "put", lvldb_database_put },
  { "set", lvldb_database_set },
  { "get", lvldb_database_get },
  { "has", lvldb_database_has },
  { "delete", lvldb_database_del },
  { "iterator", lvldb_database_iterator },
  { "write", lvldb_database_write },
  { "snapshot", lvldb_database_snapshot },
  { NULL, NULL }
};

// iterator methods
static const struct luaL_Reg lvldb_iterator_m[] = {
  { "del", lvldb_iterator_delete },
  { "seek", lvldb_iterator_seek },
  { "seekToFirst", lvldb_iterator_seek_to_first },
  { "seekToLast", lvldb_iterator_seek_to_last },
  { "valid", lvldb_iterator_valid },
  { "next", lvldb_iterator_next },
  { "key", lvldb_iterator_key },
  { "value", lvldb_iterator_val },
  { NULL, NULL }
};

// batch methods
static const luaL_Reg lvldb_batch_m[] = {
  { "__tostring", lvldb_batch_tostring },
  { "put", lvldb_batch_put },
  { "delete", lvldb_batch_del },
  { "clear", lvldb_batch_clear },
  { NULL, NULL }
};

extern "C" {

  // Initialization
  LUALIB_API int luaopen_lualeveldb(lua_State *L) {
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
