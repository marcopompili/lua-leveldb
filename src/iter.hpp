#include "llua.hpp"
#include "lvllib.hpp"
#include "utils.hpp"

using namespace leveldb;

Iterator *check_iter(lua_State *L);

int lvldb_iterator_delete(lua_State *L);
int lvldb_iterator_seek(lua_State *L);
int lvldb_iterator_seek_to_first(lua_State *L);
int lvldb_iterator_seek_to_last(lua_State *L);
int lvldb_iterator_valid(lua_State *L);
int lvldb_iterator_next(lua_State *L);
int lvldb_iterator_key(lua_State *L);
int lvldb_iterator_val(lua_State *L);
