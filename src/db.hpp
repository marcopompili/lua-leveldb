#include <iostream>
#include <sstream>
#include "utils.hpp"

leveldb::DB *check_database(lua_State*, int);

int lvldb_database_put(lua_State*);
int lvldb_database_get(lua_State*);
int lvldb_database_has(lua_State*);
int lvldb_database_set(lua_State*);
int lvldb_database_del(lua_State*);
int lvldb_database_iterator(lua_State*);
int lvldb_database_write(lua_State*);
int lvldb_database_tostring(lua_State*);
