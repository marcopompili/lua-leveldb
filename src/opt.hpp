#include <sstream>

#include "lua.hpp"
#include "lib.hpp"
#include "utils.hpp"

// Bitwise comparator impl
#include <leveldb/comparator.h>

int get_int(lua_State*, void*);
int set_int(lua_State*, void*);
int get_number(lua_State*, void*);
int set_number(lua_State*, void*);
int get_size(lua_State*, void*);
int set_size(lua_State*, void*);
int get_bool(lua_State*, void*);
int set_bool(lua_State*, void*);
int get_string(lua_State*, void*);
int set_string(lua_State*, void*);

int lvldb_options_tostring(lua_State*);
int lvldb_read_options(lua_State*);
int lvldb_read_options_tostring(lua_State*);
int lvldb_write_options(lua_State*);
int lvldb_write_options_tostring(lua_State*);
