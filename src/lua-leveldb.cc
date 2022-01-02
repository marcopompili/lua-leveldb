#include "lua-leveldb.hpp"

// Rock info
#define LUALEVELDB_VERSION "Lua-LevelDB 0.5.0"
#define LUALEVELDB_COPYRIGHT "Copyright (C) 2012-22, Lua-LevelDB by Marco Pompili (pompilimrc@gmail.com)."
#define LUALEVELDB_DESCRIPTION "Lua bindings for Google's LevelDB library."

/**
 * Basic calls to LevelDB
 * ----------------------
 */

/**
 * Opens a DB connection, based on the given options and filename.
 */
int lvldb_open(lua_State *L)
{
  leveldb::DB *db;
  leveldb::Options *opt = check_options(L, 1);
  const char *filename = luaL_checkstring(L, 2);

  leveldb::Status s = leveldb::DB::Open(*(opt), filename, &db);

  if (s.ok())
  {
    // Pushing pointer to the database in the stack
    leveldb::DB **ptr = (leveldb::DB **)lua_newuserdata(L, sizeof(void *)); //for have a  unique metatable
    ptr[0] = db;

    luaL_getmetatable(L, LVLDB_MT_DB);
    lua_setmetatable(L, -2);

    return 1;
  }
  else
  {
    luaL_error(L, "lvldb_open, %s %s\n", "Error opening db", s.ToString().c_str());

    return -1;
  }
}

/**
 * Close an open DB instance.
 */
int lvldb_close(lua_State *L)
{
  leveldb::DB *db = check_database(L, 1);

  delete db;

  return 0;
}

/**
 * Create an options object with the defaults values.
 */
int lvldb_options(lua_State *L)
{
  leveldb::Options *opt = (leveldb::Options *)lua_newuserdata(L, sizeof(leveldb::Options));

  *(opt) = leveldb::Options(); // set default values

  luaL_getmetatable(L, LVLDB_MT_OPT);
  lua_setmetatable(L, -2);

  return 1;
}

/**
 * Create a WriteBatch object.
 */
int lvldb_batch(lua_State *L)
{
  leveldb::WriteBatch **batch = (leveldb::WriteBatch **)lua_newuserdata(L, sizeof(leveldb::WriteBatch *));
  batch[0] = new leveldb::WriteBatch;

  luaL_getmetatable(L, LVLDB_MT_BATCH);
  lua_setmetatable(L, -2);

  return 1;
}

/**
 * Check for DB basic consistency.
 */
int lvldb_check(lua_State *L)
{
  leveldb::DB *db = (leveldb::DB *)lua_touserdata(L, 1);

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
int lvldb_repair(lua_State *L)
{
  std::string dbname = luaL_checkstring(L, 1);

  leveldb::Status s = leveldb::RepairDB(dbname, lvldb_opt(L, 2));

  if (s.ok())
  {
    lua_pushboolean(L, true);
  }
  else if (s.IsCorruption())
  {
    lua_pushboolean(L, false);
  }
  else
  {
    luaL_error(L, "lvldb_repair, %s %s\n", "Error repairing database", s.ToString().c_str());

    return -1;
  }

  return 1;
}

/**
 * Wrapping up the library into Lua
 * --------------------------------
 */

// main methods
static const luaL_Reg lvldb_leveldb_m[] = {
    {"open", lvldb_open},
    {"close", lvldb_close},
    {"options", lvldb_options},
    {"readOptions", lvldb_read_options},
    {"writeOptions", lvldb_write_options},
    {"repair ", lvldb_repair},
    {"batch", lvldb_batch},
    {"check", lvldb_check},
    {NULL, NULL}};

// options methods
static const luaL_Reg lvldb_options_m[] = {
    {"newBloomFilterPolicy", lvldb_options_set_bloom_filter_policy},
    {"delBloomFilterPolicy", lvldb_options_del_bloom_filter_policy},
    {"newLRUCache", lvldb_options_lru_cache_new},
    {"delLRUCache", lvldb_options_lru_cache_del},
    {NULL, NULL}};

// options meta-methods
static const luaL_Reg lvldb_options_meta[] = {
    {"__tostring", lvldb_options_tostring},
    {NULL, NULL}};

// options getters
static const Xet_reg_pre options_getters[] = {
    {"createIfMissing", get_bool, offsetof(leveldb::Options, create_if_missing)},
    {"errorIfExists", get_bool, offsetof(leveldb::Options, error_if_exists)},
    {"paranoidChecks", get_bool, offsetof(leveldb::Options, paranoid_checks)},
    {"writeBufferSize", get_size, offsetof(leveldb::Options, write_buffer_size)},
    {"maxOpenFiles", get_int, offsetof(leveldb::Options, max_open_files)},
    {"blockSize", get_size, offsetof(leveldb::Options, block_size)},
    {"blockCache", get_size, offsetof(leveldb::Options, block_cache)},
    {"blockRestartInterval", get_int, offsetof(leveldb::Options, block_restart_interval)},
    {"compression", get_int, offsetof(leveldb::Options, compression)},
    {"filterPolicy", get_size, offsetof(leveldb::Options, filter_policy)},
    {NULL, NULL}};

// options setters
static const Xet_reg_pre options_setters[] = {
    {"createIfMissing", set_bool, offsetof(leveldb::Options, create_if_missing)},
    {"errorIfExists", set_bool, offsetof(leveldb::Options, error_if_exists)},
    {"paranoidChecks", set_bool, offsetof(leveldb::Options, paranoid_checks)},
    {"writeBufferSize", set_size, offsetof(leveldb::Options, write_buffer_size)},
    {"maxOpenFiles", set_int, offsetof(leveldb::Options, max_open_files)},
    {"blockSize", set_size, offsetof(leveldb::Options, block_size)},
    {"blockRestartInterval", set_int, offsetof(leveldb::Options, block_restart_interval)},
    {"compression", set_int, offsetof(leveldb::Options, compression)},
    {NULL, NULL}};

// read options methods
static const luaL_Reg lvldb_read_options_m[] = {
    {"getSnapshot", lvldb_read_options_database_snapshot_get},
    {"delSnapshot", lvldb_read_options_database_snapshot_del},
    {NULL, NULL}};

// read options meta-methods
static const luaL_Reg lvldb_read_options_meta[] = {
    {"__tostring", lvldb_read_options_tostring},
    {NULL, NULL}};

// read options getters
static const Xet_reg_pre read_options_getters[] = {
    {"verifyChecksum", get_bool, offsetof(leveldb::ReadOptions, verify_checksums)},
    {"fillCache", get_bool, offsetof(leveldb::ReadOptions, fill_cache)},
    {NULL, NULL}};

// read options setters
static const Xet_reg_pre read_options_setters[] = {
    {"verifyChecksum", set_bool, offsetof(leveldb::ReadOptions, verify_checksums)},
    {"fillCache", set_bool, offsetof(leveldb::ReadOptions, fill_cache)},
    {NULL, NULL}};

// write options methods
static const luaL_Reg lvldb_write_options_m[] = {
    {NULL, NULL}};

// write options meta-methods
static const luaL_Reg lvldb_write_options_meta[] = {
    {"__tostring", lvldb_write_options_tostring},
    {NULL, NULL}};

// write options getters
static const Xet_reg_pre write_options_getters[] = {
    {"sync", get_bool, offsetof(leveldb::WriteOptions, sync)},
    {NULL, NULL}};

// write options setters
static const Xet_reg_pre write_options_setters[] = {
    {"sync", set_bool, offsetof(leveldb::WriteOptions, sync)},
    {NULL, NULL}};

// database methods
static const luaL_Reg lvldb_database_m[] = {
    {"__tostring", lvldb_database_tostring},
    {"put", lvldb_database_put},
    {"get", lvldb_database_get},
    {"has", lvldb_database_has},
    {"delete", lvldb_database_del},
    {"iterator", lvldb_database_iterator},
    {"write", lvldb_database_write},
    {NULL, NULL}};

// iterator methods
static const struct luaL_Reg lvldb_iterator_m[] = {
    {"del", lvldb_iterator_del},
    {"seek", lvldb_iterator_seek},
    {"seekToFirst", lvldb_iterator_seek_to_first},
    {"seekToLast", lvldb_iterator_seek_to_last},
    {"valid", lvldb_iterator_valid},
    {"next", lvldb_iterator_next},
    {"key", lvldb_iterator_key},
    {"value", lvldb_iterator_val},
    {NULL, NULL}};

// batch methods
static const luaL_Reg lvldb_batch_m[] = {
    {"__tostring", lvldb_batch_tostring},
    {"put", lvldb_batch_put},
    {"delete", lvldb_batch_del},
    {"clear", lvldb_batch_clear},
    {"release", lvldb_batch_release},
    {NULL, NULL}};

extern "C"
{
  // Initialization
  LUALIB_API int luaopen_lualeveldb(lua_State *L)
  {
    // LevelDB functions
#if LUA_VERSION_NUM > 501
    lua_newtable(L);
    luaL_setfuncs(L, lvldb_leveldb_m, 0);
#else
    luaL_register(L, "lualeveldb", lvldb_leveldb_m);
#endif
    // register module information
    lua_pushliteral(L, LUALEVELDB_VERSION);
    lua_setfield(L, -2, "_VERSION");

    lua_pushliteral(L, LUALEVELDB_COPYRIGHT);
    lua_setfield(L, -2, "_COPYRIGHT");

    lua_pushliteral(L, LUALEVELDB_DESCRIPTION);
    lua_setfield(L, -2, "_DESCRIPTION");

    // compression flags
    lua_pushinteger(L, leveldb::kNoCompression);
    lua_setfield(L, -2, "noCompression");

    lua_pushinteger(L, leveldb::kSnappyCompression);
    lua_setfield(L, -2, "snappyCompression");

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
