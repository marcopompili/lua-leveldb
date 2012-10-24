#include <leveldb/db.h>

// LevelDB atomic operation library
#include <leveldb/write_batch.h>

extern "C" {

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define LUALEVELDB_BATCH_MT		"leveldb.batch"

using namespace leveldb;

static WriteBatch *check_batch(lua_State *L) {
	void *ud = luaL_checkudata(L, 1, LUALEVELDB_BATCH_MT);
	luaL_argcheck(L, ud != NULL, 1, "'batch' expected");

	return (WriteBatch *) ud;
}

static int lvldb_batch_put(lua_State *L) {
	WriteBatch *batch = check_batch(L);

	const char *key = luaL_checkstring(L, 2);
	const char *value = luaL_checkstring(L, 3);

	batch->Put(key, value);

	return 0;
}

static int lvldb_batch_del(lua_State *L) {
	WriteBatch *batch = check_batch(L);

	const char *key = luaL_checkstring(L, 2);

	batch->Delete(key);

	return 0;
}

static const struct luaL_reg leveldb_btch [] = {
		{ "put",  lvldb_batch_put },
		{ "delete", lvldb_batch_del },
		{ NULL, NULL }
};

}
