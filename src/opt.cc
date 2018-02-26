#include "opt.hpp"

using namespace std;
using namespace leveldb;

/**
 * Basic setters and getters
 * -------------------------
 * Used to set values in the Options
 */
int get_int(lua_State *L, void *v) {
  lua_pushnumber(L, *(int*) v);
  return 1;
}

int set_int(lua_State *L, void *v) {
#if LUA_VERSION_NUM < 503
  *(int*) v = luaL_checkint(L, 3);
#else
  *(int*) v = luaL_checkinteger(L, 3);
#endif
  return 0;
}


int get_number(lua_State *L, void *v) {
  lua_pushnumber(L, *(lua_Number*) v);
  return 1;
}

int set_number(lua_State *L, void *v) {
  *(lua_Number*) v = luaL_checknumber(L, 3);
  return 0;
}

//TODO test it
int get_size(lua_State *L, void *v) {
  lua_pushinteger(L, *(lua_Integer*) v);
  return 1;
}

int set_size(lua_State *L, void *v) {
  *(lua_Integer*) v = luaL_checkinteger(L, 3);
  return 0;
}

int get_bool(lua_State *L, void *v) {
  lua_pushboolean(L, *(bool*) v);
  return 1;
}

int set_bool(lua_State *L, void *v) {
  *(bool*) v = lua_toboolean(L, 3);
  return 0;
}

//TODO test it
int get_string(lua_State *L, void *v) {
  lua_pushstring(L, (char*) v);
  return 1;
}

//TODO test it
int set_string(lua_State *L, void *v) {
  v = (char*)lua_tostring(L, 3);
  return 0;
}

int get_filter_policy(lua_State *L, void *v) {
  FilterPolicy *fp = (FilterPolicy*) v;
  lua_pushlightuserdata(L, fp);
  return 1;
}

int set_filter_policy(lua_State *L, void *v) {
  v = (FilterPolicy*)lua_touserdata(L, 3);
  return 0;
}

/**
 * To string for the options type.
 */
int lvldb_options_tostring(lua_State *L) {
  Options *opt = check_options(L, 1);

  ostringstream oss (ostringstream::out);
  oss << "Comparator: " << opt->comparator->Name()
      << "\nCreate if missing: " << bool_tostring(opt->create_if_missing)
      << "\nError if exists: " << bool_tostring(opt->error_if_exists)
      << "\nParanoid checks: " << bool_tostring(opt->paranoid_checks)
      << "\nEnvironment: " << pointer_tostring(opt->env)
      << "\nInfo log: " << pointer_tostring(opt->info_log)
      << "\nWrite buffer size: " << opt->write_buffer_size
      << "\nMax open files: " << opt->max_open_files
      << "\nBlock cache: " << pointer_tostring(opt->block_cache)
      << "\nBlock size: " << opt->block_size
      << "\nBlock restart interval: " << opt->block_restart_interval
      << "\nCompression: " << (opt->compression == 1 ? "Snappy Compression" : "No Compression")
      << "\nReuse logs: " << bool_tostring(opt->reuse_logs)
      << "\nFilter policy: " << filter_tostring(opt->filter_policy) << endl;

  lua_pushstring(L, oss.str().c_str());

  return 1;
}

/**
 * Create a ReadOptions object.
 */
int lvldb_read_options(lua_State *L) {
  ReadOptions *ropt = (ReadOptions*)lua_newuserdata(L, sizeof(ReadOptions));

  *(ropt) = ReadOptions(); // set default values

  luaL_getmetatable(L, LVLDB_MT_ROPT);
  lua_setmetatable(L, -2);

  return 1;
}

/**
 * To string function for the ReadOptions object.
 */
int lvldb_read_options_tostring(lua_State *L) {
  ReadOptions *ropt = check_read_options(L, 1);

  ostringstream oss (ostringstream::out);
  oss << "Verify checksum: " << bool_tostring(ropt->verify_checksums)
      << "\nFill cache: " << bool_tostring(ropt->fill_cache)
      << "\nSnapshot: " << ropt->snapshot << endl;

  lua_pushstring(L, oss.str().c_str());

  return 1;
}

/**
 * Create a WriteOptions object.
 */
int lvldb_write_options(lua_State *L) {
  WriteOptions *wopt = (WriteOptions*)lua_newuserdata(L, sizeof(WriteOptions));

  *(wopt) = WriteOptions(); // set default values

  luaL_getmetatable(L, LVLDB_MT_WOPT);
  lua_setmetatable(L, -2);

  return 1;
}

/**
 * To string function for the WriteOptions object.
 */
int lvldb_write_options_tostring(lua_State *L) {
  WriteOptions *wopt = check_write_options(L, 1);

  ostringstream oss (ostringstream::out);
  oss << "Sync: " << bool_tostring(wopt->sync) << endl;

  lua_pushstring(L, oss.str().c_str());

  return 1;
}
