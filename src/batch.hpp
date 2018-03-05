#include <sstream>

#include "llua.hpp"
#include "lvllib.hpp"
#include "utils.hpp"

using namespace leveldb;

int lvldb_batch_tostring(lua_State *L);
int lvldb_batch_put(lua_State *L);
int lvldb_batch_del(lua_State *L);
int lvldb_batch_clear(lua_State *L);
