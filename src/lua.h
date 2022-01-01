#include <stdio.h>

#include "lua.hpp"

typedef int (*Xet_func)(lua_State *L, void *v);

/* member info for get and set handlers */
typedef const struct {
  const char *name; // member name
  Xet_func func;    // get or set function for type of member
  size_t offset;    // offset of member within your_t
} Xet_reg_pre;

typedef Xet_reg_pre * Xet_reg;

void Xet_add(lua_State *L, Xet_reg l);
int Xet_call(lua_State *L);
int index_handler(lua_State *L);
int newindex_handler(lua_State *L);
void init_complex_metatable(lua_State *L, const char *metatable_name, const luaL_Reg methods[], const luaL_Reg metamethods[], const Xet_reg_pre getters[], const Xet_reg_pre setters[]);
void init_metatable(lua_State *L, const char *metatable, const struct luaL_Reg lib[]);
