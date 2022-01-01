#include "utils.hpp"

/**
 * Converts a Lua parameter into a LevelDB's slice.
 * Every data stored from Lua is stored with this format.
 * This functions manage type conversion between Lua's
 * types and the LevelDB's Slice.
 */
leveldb::Slice lua_to_slice(lua_State *L, int i)
{
  // From: leveldb.org
  // "Arbitrary byte arrays
  //  Both keys and values are treated as simple arrays of bytes"
  const char *data = luaL_checkstring(L, i);

  return leveldb::Slice(data, strlen(data));
}

/**
 * Converts a LevelDB's slice into a Lua value.
 * This the inverse operation of lua_to_slice.
 * It's used with the get method, the iterator
 * uses another function for conversion to Lua.
 */
int string_to_lua(lua_State *L, std::string value)
{
  // From: leveldb.org
  // "Arbitrary byte arrays
  //  Both keys and values are treated as simple arrays of bytes"
  lua_pushlstring(L, value.c_str(), value.length());

  return 1;
}

/**
 * Stringify a boolean
 */
std::string bool_tostring(int boolean)
{
  return boolean == 1 ? "true" : "false";
}

/**
 * Stringify a pointer
 */
std::string pointer_tostring(void *p)
{
  std::ostringstream oss(std::ostringstream::out);

  if (p != NULL)
    oss << &p;
  else
    oss << "NULL";

  return oss.str().c_str();
}

/**
 * Stringify a filter
 */
std::string filter_tostring(const leveldb::FilterPolicy *fp)
{
  if (fp != nullptr)
  {
    return fp == 0 ? "NULL" : fp->Name();
  }
  else
  {
    return "NULL";
  }
}

/**
 *  Check for a DB type.
 */
leveldb::DB *check_database(lua_State *L, int index)
{
  // UD-type: light meta @ lvldb_open()
  leveldb::DB **ud = (leveldb::DB **)luaL_checkudata(L, index, LVLDB_MT_DB);
  luaL_argcheck(L, ud != NULL, index, "'database' expected");

  return ud[0];
}

/**
 * Check for an Options type.
 */
leveldb::Options *check_options(lua_State *L, int index)
{
  // UD-type: complex metatable @ luaopen_leveldb()
  leveldb::Options *ud = (leveldb::Options *)luaL_checkudata(L, index, LVLDB_MT_OPT);
  luaL_argcheck(L, ud != NULL, index, "'options' expected");

  return ud;
}

/**
 * Check for a ReadOptions type.
 */
leveldb::ReadOptions *check_read_options(lua_State *L, int index)
{
  // UD-type: complex metatable @ luaopen_leveldb()
  leveldb::ReadOptions *ud = (leveldb::ReadOptions *)luaL_checkudata(L, index, LVLDB_MT_ROPT);
  luaL_argcheck(L, ud != NULL, index, "'writeOptions' expected");

  return ud;
}

/**
 * Check for a WriteOptions type.
 */
leveldb::WriteOptions *check_write_options(lua_State *L, int index)
{
  // UD-type: complex metatable @ luaopen_leveldb()
  leveldb::WriteOptions *ud = (leveldb::WriteOptions *)luaL_checkudata(L, index, LVLDB_MT_WOPT);
  luaL_argcheck(L, ud != NULL, index, "'readOptions' expected");

  return ud;
}

/**
 * Check for a WriteBatch type.
 */
leveldb::WriteBatch *check_writebatch(lua_State *L, int index)
{
  leveldb::WriteBatch **ud = (leveldb::WriteBatch **)luaL_checkudata(L, index, LVLDB_MT_BATCH);
  luaL_argcheck(L, ud != NULL, index, "'batch' expected");

  return ud[0];
}
