#include <leveldb/db.h>

extern "C" {

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define LUALEVELDB_IT_MT		"leveldb.iterator"

using namespace leveldb;

static Iterator *check_it(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, LUALEVELDB_IT_MT);
	luaL_argcheck(L, ud != NULL, 1, "'iterator' expected");

	return (Iterator *) ud;
}

static int lvldb_it_valid(lua_State *L) {
	Iterator *it = check_it(L);

	lua_pushboolean(L, it->Valid());

	return 1;
}

static int lvldb_it_next(lua_State *L) {
	Iterator *it = check_it(L);

	it->Next();

	return 0;
}

// object oriented methods for iterator
static const struct luaL_reg leveldb_it [] = {
		{ "valid", lvldb_it_valid },
		{ "next" , lvldb_it_next },
		{ NULL, NULL }
};

}
