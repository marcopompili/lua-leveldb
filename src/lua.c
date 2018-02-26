#include "lua.h"

void Xet_add(lua_State *L, Xet_reg l) {
  for (; l->name; l++) {
    lua_pushstring(L, l->name);
    lua_pushlightuserdata(L, (void*) l);
    lua_settable(L, -3);
  }
}

int Xet_call(lua_State *L) {
  Xet_reg m = (Xet_reg) lua_touserdata(L, -1);
  lua_pop(L, 1);

  luaL_checktype(L, 1, LUA_TUSERDATA);

  return m->func(L, (void*) ((char*) lua_touserdata(L, 1) + m->offset));
}

int index_handler(lua_State *L) {
  // stack has userdata, index
  lua_pushvalue(L, 2);                // dup index
  lua_rawget(L, lua_upvalueindex(1)); // lookup member by name

  if (!lua_islightuserdata(L, -1)) {
    lua_pop(L, 1);                        // drop value
    lua_pushvalue(L, 2);                  // dup index
    lua_gettable(L, lua_upvalueindex(2)); // else try methods

    if (lua_isnil(L, -1))                 // invalid member
      luaL_error(L, "cannot get member '%s'", lua_tostring(L, 2));

    return 1;
  }

  return Xet_call(L); // call get function
}

int newindex_handler(lua_State *L) {
  // stack has userdata, index, value
  lua_pushvalue(L, 2);                // dup index
  lua_rawget(L, lua_upvalueindex(1)); // lookup member by name

  if (!lua_islightuserdata(L, -1))    // invalid member
    luaL_error(L, "cannot set member '%s'", lua_tostring(L, 2));

  return Xet_call(L);                 // call set function
}

/**
 *
 */
void init_complex_metatable(lua_State *L, const char *metatable_name, const luaL_Reg methods[], const luaL_Reg metamethods[], const Xet_reg_pre getters[], const Xet_reg_pre setters[]) {

  // create methods table, & add it to the table of globals
  lua_newtable(L);

#if LUA_VERSION_NUM > 501
  luaL_setfuncs(L, methods, 0);
#else
  luaL_register(L, NULL, methods);
#endif

  int methods_stack = lua_gettop(L);

  // create meta-table for object, & add it to the registry
  luaL_newmetatable(L, metatable_name);

  // fill meta-table
#if LUA_VERSION_NUM > 501
  luaL_setfuncs(L, metamethods, 0);
#else
  luaL_register(L, NULL, metamethods);
#endif
  
  int metatable_stack = lua_gettop(L);

  lua_pushliteral(L, "__metatable");
  lua_pushvalue(L, methods_stack);  // duplicate methods table
  lua_rawset(L, metatable_stack);   // hide meta-table

  lua_pushliteral(L, "__index");
  lua_pushvalue(L, metatable_stack);
  Xet_add(L, getters);
  lua_pushvalue(L, methods_stack);
  lua_pushcclosure(L, index_handler, 2);
  lua_rawset(L, metatable_stack);

  lua_pushliteral(L, "__newindex");
  lua_newtable(L);
  Xet_add(L, setters);
  lua_pushcclosure(L, newindex_handler, 1);
  lua_rawset(L, metatable_stack);

  lua_settop(L, methods_stack-1);
}

/**
 * Procedure for adding a meta-table into the stack.
 */
void metatable_func(lua_State *L, const char *metatable, const struct luaL_Reg lib[]) {

  // let's build the function meta-table
  if (luaL_newmetatable(L, metatable) == 0)
    fprintf(stderr, "Warning: metatable %s is already set", metatable);

  lua_pushstring(L, "__index"); // meta-table already in the stack
  lua_pushvalue(L, -2);         // push the meta-table
  lua_settable(L, -3);          // meta-table.__index = meta-table

  // meta-table already on the stack
#if LUA_VERSION_NUM > 501
  luaL_setfuncs(L, lib, 0);
#else
  luaL_register(L, NULL, lib);
#endif
  lua_pop(L, 1);
}


void init_metatable(lua_State *L, const char *metatable, const struct luaL_Reg lib[]) {

  // let's build the function meta-table
  if (luaL_newmetatable(L, metatable) == 0)
    fprintf(stderr, "Warning: metatable %s is already set", metatable);

  lua_pushstring(L, "__index"); // meta-table already in the stack
  lua_pushvalue(L, -2); // push the meta-table
  lua_settable(L, -3); // meta-table.__index = meta-table

  // meta-table already on the stack
#if LUA_VERSION_NUM > 501
  luaL_setfuncs(L, lib, 0);
#else
  luaL_register(L, NULL, lib);
#endif
  lua_pop(L, 1);
}
