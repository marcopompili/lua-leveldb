#include <sstream>

#include "lualibs.hpp"
#include "lvldblib.hpp"

// Lua Meta-tables names
#define LVLDB_MOD_NAME		"leveldb"
#define LVLDB_MT_OPT		"leveldb.opt"
#define LVLDB_MT_ROPT		"leveldb.ropt"
#define LVLDB_MT_WOPT		"leveldb.wopt"
#define LVLDB_MT_DB		"leveldb.db"
#define LVLDB_MT_ITER		"leveldb.iter"
#define LVLDB_MT_BATCH		"leveldb.btch"

using namespace std;
using namespace leveldb;

Slice lua_to_slice(lua_State *L, int i);
int string_to_lua(lua_State *L, string value);
string bool_tostring(int boolean);
string pointer_tostring(void *p);
string filter_tostring(const FilterPolicy *fp);

Options *check_options(lua_State *L, int index);
ReadOptions *check_read_options(lua_State *L, int index);
WriteOptions *check_write_options(lua_State *L, int index);

WriteBatch *check_writebatch(lua_State *L, int index);

/**
 * Type checking macros.
 */
#define lvldb_opt(L, l) ( lua_gettop(L) >= l ? *(check_options(L, l)) : Options() )
#define lvldb_ropt(L, l) ( lua_gettop(L) >= l ? *(check_read_options(L, l)) : ReadOptions() )
#define lvldb_wopt(L, l) ( lua_gettop(L) >= l ? *(check_write_options(L, l)) : WriteOptions() )
