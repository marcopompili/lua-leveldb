#include <sstream>

#include "lua.hpp"
#include "lib.hpp"

// Lua Meta-tables names
#define LVLDB_MOD_NAME		"leveldb"
#define LVLDB_MT_OPT		"leveldb.opt"
#define LVLDB_MT_ROPT		"leveldb.ropt"
#define LVLDB_MT_WOPT		"leveldb.wopt"
#define LVLDB_MT_DB		"leveldb.db"
#define LVLDB_MT_ITER		"leveldb.iter"
#define LVLDB_MT_BATCH		"leveldb.btch"

leveldb::Slice lua_to_slice(lua_State*, int);
int string_to_lua(lua_State*, std::string);
std::string bool_tostring(int);
std::string pointer_tostring(void*);
std::string filter_tostring(const leveldb::FilterPolicy*);

leveldb::Options *check_options(lua_State*, int);
leveldb::ReadOptions *check_read_options(lua_State*, int);
leveldb::WriteOptions *check_write_options(lua_State*, int);

leveldb::WriteBatch *check_writebatch(lua_State*, int);

/**
 * Type checking macros.
 */
#define lvldb_opt(L, l) ( lua_gettop(L) >= l ? *(check_options(L, l)) : leveldb::Options() )
#define lvldb_ropt(L, l) ( lua_gettop(L) >= l ? *(check_read_options(L, l)) : leveldb::ReadOptions() )
#define lvldb_wopt(L, l) ( lua_gettop(L) >= l ? *(check_write_options(L, l)) : leveldb::WriteOptions() )
