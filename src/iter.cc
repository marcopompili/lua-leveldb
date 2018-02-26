#include "iter.hpp"

using namespace leveldb;

/**
 * Check for an Iterator type.
 */
Iterator *check_iter(lua_State *L) {
  luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
  // UD-type: light meta @ lvldb_database_iterator()
	
  // LuaJIT doesn't support luaL_checkudata on light userdata
  //void *ud = luaL_checkudata(L, 1, LVLDB_MT_ITER);
	
  Iterator *ud = (Iterator*) lua_touserdata(L, 1);
  luaL_argcheck(L, ud != NULL, 1, "'iterator' expected");
  luaL_argcheck(L, lua_islightuserdata(L, 1), 1, "'iterator' expected");

  return ud;
}

int lvldb_iterator_delete(lua_State *L) {
  Iterator *iter = check_iter(L);

  delete iter;

  return 0;
}

int lvldb_iterator_seek(lua_State *L) {
  Iterator *iter = check_iter(L);

  std::string start = luaL_checkstring(L, 2);

  iter->Seek(start);

  return 0;
}

int lvldb_iterator_seek_to_first(lua_State *L) {
  Iterator *iter = check_iter(L);

  iter->SeekToFirst();

  return 0;
}

int lvldb_iterator_seek_to_last(lua_State *L) {
  Iterator *iter = check_iter(L);

  iter->SeekToLast();

  return 0;
}

int lvldb_iterator_valid(lua_State *L) {
  Iterator *iter = check_iter(L);

  lua_pushboolean(L, iter->Valid());

  return 1;
}

int lvldb_iterator_next(lua_State *L) {
  Iterator *iter = check_iter(L);

  iter->Next();

  return 0;
}

int lvldb_iterator_key(lua_State *L) {
  Iterator *iter = check_iter(L);
  Slice key = iter->key();

  return string_to_lua(L, key.ToString());
}

int lvldb_iterator_val(lua_State *L) {
  Iterator *iter = check_iter(L);
  Slice val = iter->value();

  return string_to_lua(L, val.ToString());
}
