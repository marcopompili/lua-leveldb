#include "batch.hpp"

/**
 * LevelDB atomic batch support
 * ----------------------------
 */

/**
 * To string function for the WriteBatch object.
 */
int lvldb_batch_tostring(lua_State *L) {
  //leveldb::WriteBatch *batch = check_writebatch(L, 1);

  std::ostringstream oss (std::ostringstream::out);
  oss << "Batch" << std::endl;
  
  lua_pushstring(L, oss.str().c_str());

  return 1;
}

/**
 * Put a key,value into the batch.
 */
int lvldb_batch_put(lua_State *L) {
  leveldb::WriteBatch *batch = check_writebatch(L, 1);
  leveldb::Slice key = lua_to_slice(L, 2);
  leveldb::Slice value = lua_to_slice(L, 3);

  batch->Put(key, value);

  return 0;
}

/**
 * Delete a key from the batch.
 */
int lvldb_batch_del(lua_State *L) {
  leveldb::WriteBatch *batch = check_writebatch(L, 1);

  leveldb::Slice key = lua_to_slice(L, 2);

  batch->Delete(key);

  return 0;
}

/**
 * Clear the whole batch.
 */
int lvldb_batch_clear(lua_State *L) {
  leveldb::WriteBatch *batch = check_writebatch(L, 1);

  batch->Clear();

  return 0;
}

/**
 * Free leveldb::WriteBatch 
 */
int lvldb_batch_release(lua_State *L) {
  leveldb::WriteBatch *batch = check_writebatch(L, 1);
  delete batch;
  return 0;
}