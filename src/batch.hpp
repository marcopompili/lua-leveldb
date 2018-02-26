#include <sstream>

#include "lua.hpp"
#include "lib.hpp"
#include "utils.hpp"

using namespace leveldb;

int lvldb_batch_tostring(lua_State *L);
int lvldb_batch_put(lua_State *L);
int lvldb_batch_del(lua_State *L);
int lvldb_batch_clear(lua_State *L);
