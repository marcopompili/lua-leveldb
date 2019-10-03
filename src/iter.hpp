#include "lua.hpp"
#include "lib.hpp"
#include "utils.hpp"

leveldb::Iterator *check_iter(lua_State*);

int lvldb_iterator_delete(lua_State*);
int lvldb_iterator_seek(lua_State*);
int lvldb_iterator_seek_to_first(lua_State*);
int lvldb_iterator_seek_to_last(lua_State*);
int lvldb_iterator_valid(lua_State*);
int lvldb_iterator_next(lua_State*);
int lvldb_iterator_key(lua_State*);
int lvldb_iterator_val(lua_State*);
