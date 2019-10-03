#include "utils.hpp"

/**
 * Converts a Lua parameter into a LevelDB's slice.
 * Every data stored from Lua is stored with this format.
 * This functions manage type conversion between Lua's
 * types and the LevelDB's Slice.
 */
leveldb::Slice lua_to_slice(lua_State *L, int i) {
  // Note:
  // http://leveldb.org/ says:
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
int string_to_lua(lua_State *L, std::string value) {
  // Note:
  // http://leveldb.org/ says:
  // "Arbitrary byte arrays
  //  Both keys and values are treated as simple arrays of bytes"
  lua_pushlstring(L, value.c_str(), value.length());

  return 1;
}

/**
 * Stringify a boolean
 */
std::string bool_tostring(int boolean) {
  return boolean == 1 ? "true" : "false";
}

/**
 * Stringify a pointer
 */
std::string pointer_tostring(void *p) {
  std::ostringstream oss (std::ostringstream::out);

  if (p != NULL)
    oss << &p;
  else
    oss << "NULL";

  return oss.str().c_str();
}

/**
 * Stringify a filter
 */
std::string filter_tostring(const leveldb::FilterPolicy *fp) {
  return fp == 0 ? "NULL" : fp->Name();
}

/**
 * Check for an Options type.
 */
leveldb::Options *check_options(lua_State *L, int index) {
  leveldb::Options *opt;
  luaL_checktype(L, index, LUA_TUSERDATA);
  // UD-type: complex metatable @ luaopen_leveldb()
  opt = (leveldb::Options*)luaL_checkudata(L, index, LVLDB_MT_OPT);
  luaL_argcheck(L, opt != NULL, index, "'options' expected");

  return opt;
}

/**
 * Check for a ReadOptions type.
 */
leveldb::ReadOptions *check_read_options(lua_State *L, int index) {
  luaL_checktype(L, index, LUA_TUSERDATA);
  // UD-type: complex metatable @ luaopen_leveldb()
  void *ud = luaL_checkudata(L, index, LVLDB_MT_ROPT);
  luaL_argcheck(L, ud != NULL, index, "'writeOptions' expected");

  return (leveldb::ReadOptions *) ud;
}

/**
 * Check for a WriteOptions type.
 */
leveldb::WriteOptions *check_write_options(lua_State *L, int index) {
  luaL_checktype(L, index, LUA_TUSERDATA);
  // UD-type: complex metatable @ luaopen_leveldb()
  void *ud = luaL_checkudata(L, index, LVLDB_MT_WOPT);
  luaL_argcheck(L, ud != NULL, index, "'readOptions' expected");

  return (leveldb::WriteOptions *) ud;
}

/**
 * Check for a WriteBatch type.
 */
leveldb::WriteBatch *check_writebatch(lua_State *L, int index) {
  luaL_checktype(L, index, LUA_TUSERDATA);
  // UD-type: light meta @ lvldb_batch()
  // LuaJIT doesn't support luaL_checkudata on light userdata
  // void *ud = luaL_checkudata(L, 1, LVLDB_MT_BATCH);
  leveldb::WriteBatch *ud = (leveldb::WriteBatch*) lua_touserdata(L, 1);
  luaL_argcheck(L, ud != NULL, index, "'batch' expected");
  luaL_argcheck(L, lua_islightuserdata(L, index), index, "'iterator' expected");

  return ud;
}
