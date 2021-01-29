#include "iter.hpp"

/**
 * Check for an Iterator type.
 */
leveldb::Iterator *check_iter(lua_State *L) {
  leveldb::Iterator **ud = (leveldb::Iterator**) luaL_checkudata(L, 1, LVLDB_MT_ITER);
  luaL_argcheck(L, ud != NULL, 1, "'iterator' expected");
  return ud[0];
}

int lvldb_iterator_del(lua_State *L) {
  leveldb::Iterator *iter = check_iter(L);

  delete iter;

  return 0;
}

int lvldb_iterator_seek(lua_State *L) {
  leveldb::Iterator *iter = check_iter(L);

  std::string start = luaL_checkstring(L, 2);

  iter->Seek(start);

  return 0;
}

int lvldb_iterator_seek_to_first(lua_State *L) {
  leveldb::Iterator *iter = check_iter(L);

  iter->SeekToFirst();

  return 0;
}

int lvldb_iterator_seek_to_last(lua_State *L) {
  leveldb::Iterator *iter = check_iter(L);

  iter->SeekToLast();

  return 0;
}

int lvldb_iterator_valid(lua_State *L) {
  leveldb::Iterator *iter = check_iter(L);

  lua_pushboolean(L, iter->Valid());

  return 1;
}

int lvldb_iterator_next(lua_State *L) {
  leveldb::Iterator *iter = check_iter(L);

  iter->Next();

  return 0;
}

int lvldb_iterator_key(lua_State *L) {
  leveldb::Iterator *iter = check_iter(L);
  leveldb::Slice key = iter->key();

  return string_to_lua(L, key.ToString());
}

int lvldb_iterator_val(lua_State *L) {
  leveldb::Iterator *iter = check_iter(L);
  leveldb::Slice val = iter->value();

  return string_to_lua(L, val.ToString());
}
