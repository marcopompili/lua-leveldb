#include <iostream>
#include <sstream>

extern "C" {
  #include "lutils.h"
}

#include "lvldblib.hpp"
#include "batch.hpp"
#include "db.hpp"
#include "iter.hpp"
#include "opt.hpp"

int lvldb_open(lua_State *L);
int lvldb_close(lua_State *L);
int lvldb_options(lua_State *L);
int lvldb_batch(lua_State *L);
int lvldb_check(lua_State *L);
int lvldb_repair(lua_State *L);
int lvldb_bloom_filter_policy(lua_State *L);

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
