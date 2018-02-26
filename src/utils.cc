#include "utils.hpp"

using namespace std;
using namespace leveldb;

/**
 * Converts a Lua parameter into a LevelDB's slice.
 * Every data stored from Lua is stored with this format.
 * This functions manage type conversion between Lua's
 * types and the LevelDB's Slice.
 */
Slice lua_to_slice(lua_State *L, int i) {
  // Note:
  // http://leveldb.org/ says:
  // "Arbitrary byte arrays
  //  Both keys and values are treated as simple arrays of bytes"
  const char *data = luaL_checkstring(L, i);

  return Slice(data, strlen(data));
}

/**
 * Converts a LevelDB's slice into a Lua value.
 * This the inverse operation of lua_to_slice.
 * It's used with the get method, the iterator
 * uses another function for conversion to Lua.
 */
int string_to_lua(lua_State *L, string value) {
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
string bool_tostring(int boolean) {
  return boolean == 1 ? "true" : "false";
}

/**
 * Stringify a pointer
 */
string pointer_tostring(void *p) {
  ostringstream oss (ostringstream::out);

  if (p != nullptr)
    oss << &p;
  else
    oss << "NULL";

  return oss.str().c_str();
}

/**
 * Stringify a filter
 */
string filter_tostring(const FilterPolicy *fp) {
  return fp == 0 ? "NULL" : fp->Name();
}

/**
 * Check for an Options type.
 */
Options *check_options(lua_State *L, int index) {
  Options *opt;
  luaL_checktype(L, index, LUA_TUSERDATA);
  // UD-type: complex metatable @ luaopen_leveldb()
  opt = (Options*)luaL_checkudata(L, index, LVLDB_MT_OPT);
  luaL_argcheck(L, opt != NULL, index, "'options' expected");

  return opt;
}

/**
 * Check for a ReadOptions type.
 */
ReadOptions *check_read_options(lua_State *L, int index) {
  luaL_checktype(L, index, LUA_TUSERDATA);
  // UD-type: complex metatable @ luaopen_leveldb()
  void *ud = luaL_checkudata(L, index, LVLDB_MT_ROPT);
  luaL_argcheck(L, ud != NULL, index, "'writeOptions' expected");

  return (ReadOptions *) ud;
}

/**
 * Check for a WriteOptions type.
 */
WriteOptions *check_write_options(lua_State *L, int index) {
  luaL_checktype(L, index, LUA_TUSERDATA);
  // UD-type: complex metatable @ luaopen_leveldb()
  void *ud = luaL_checkudata(L, index, LVLDB_MT_WOPT);
  luaL_argcheck(L, ud != NULL, index, "'readOptions' expected");

  return (WriteOptions *) ud;
}

/**
 * Check for a WriteBatch type.
 */
WriteBatch *check_writebatch(lua_State *L, int index) {
  luaL_checktype(L, index, LUA_TUSERDATA);
  // UD-type: light meta @ lvldb_batch()
  // LuaJIT doesn't support luaL_checkudata on light userdata
  // void *ud = luaL_checkudata(L, 1, LVLDB_MT_BATCH);
  WriteBatch *ud = (WriteBatch*) lua_touserdata(L, 1);
  luaL_argcheck(L, ud != NULL, index, "'batch' expected");
  luaL_argcheck(L, lua_islightuserdata(L, index), index, "'iterator' expected");

  return ud;
}
