#include <iostream>
#include <sstream>

#include "lua.hpp"
#include "lib.hpp"
#include "utils.hpp"

using namespace leveldb;

DB *check_database(lua_State *L, int index);

int lvldb_database_put(lua_State *L);
int lvldb_database_get(lua_State *L);
int lvldb_database_has(lua_State *L);
int lvldb_database_set(lua_State *L);
int lvldb_database_del(lua_State *L);
int lvldb_database_iterator(lua_State *L);
int lvldb_database_write(lua_State *L);
int lvldb_database_snapshot(lua_State *L);
int lvldb_database_tostring(lua_State *L);
