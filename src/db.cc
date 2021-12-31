#include "db.hpp"

/**
 *  Check for a DB type.
 */
leveldb::DB *check_database(lua_State *L, int index)
{
  // UD-type: light meta @ lvldb_open()
  leveldb::DB **ud = (leveldb::DB **)luaL_checkudata(L, index, LVLDB_MT_DB);
  luaL_argcheck(L, ud != NULL, index, "'database' expected");

  return ud[0];
}

/**
 * Data Ops
 * ---------------
 * Data operations are binded to a DB instance, the first parameter
 * is always a DB but the notation used in Lua is db:put, db:get etc.
 */

/**
 * Method that put a key,value into a DB.
 * --------------------------------------$
 * Inserts a key,value pair in the LevelDB Slice format.
 * This DB related method returns in Lua:
 *   * True in case of correct insertion.
 *   * False in case of error.
 */
int lvldb_database_put(lua_State *L)
{
  leveldb::DB *db = check_database(L, 1);

  leveldb::Slice key = lua_to_slice(L, 2);
  leveldb::Slice value = lua_to_slice(L, 3);

  leveldb::Status s = db->Put(lvldb_wopt(L, 4), key, value);

  if (s.ok())
  {
    lua_pushboolean(L, true);
    return 1;
  }
  else
  {
    //std::cerr << "Error inserting key/value: " << s.ToString() << std::endl;
    lua_pushboolean(L, false);
    string_to_lua(L, s.ToString());
  }

  return 2;
}

/**
 * Method that get a value with the given key from a DB.
 * -----------------------------------------------------$
 * This DB related method returns in Lua:
 *  * A string in case of success.
 *  * False in case of error.
 */
int lvldb_database_get(lua_State *L)
{
  leveldb::DB *db = check_database(L, 1);

  leveldb::Slice key = lua_to_slice(L, 2);
  std::string value;

  leveldb::Status s = db->Get(lvldb_ropt(L, 3), key, &value);

  if (s.ok())
  {
    string_to_lua(L, value);
    return 1;
  }
  else
  {
    //std::cerr << "Error getting value (get): " << s.ToString() << std::endl;
    lua_pushboolean(L, false);
    string_to_lua(L, s.ToString());
  }

  return 2;
}

/**
 * Method that checks the given key's existence from a DB.
 * -----------------------------------------------------$
 * This DB related method returns in Lua:
 *  * True if key is in DB
 *  * False in case of error, or key not exists.
 */
int lvldb_database_has(lua_State *L)
{
  leveldb::DB *db = check_database(L, 1);

  leveldb::Slice key = lua_to_slice(L, 2);
  std::string value;

  leveldb::Status s = db->Get(lvldb_ropt(L, 3), key, &value);

  if (s.ok())
  {
    lua_pushboolean(L, true);
  }
  else
  {
    lua_pushboolean(L, false);
  }

  return 1;
}

/**
 *
 */
int lvldb_database_set(lua_State *L)
{
  leveldb::DB *db = check_database(L, 1);
  leveldb::Slice value = lua_to_slice(L, 2);

  // #ifdef __x86_64__ || __ppc64__
#ifdef __x86_64__
  uint64_t i = 1;
#else
  int i = 1;
#endif

  bool found = false;

  leveldb::Iterator *it = db->NewIterator(lvldb_ropt(L, 3));

  /*
   *  initialization from the end, usually faster
   *  on the long run.
   */
  for (it->SeekToLast(); it->Valid(); it->Prev())
  {
    if (value == it->value())
    {
      found = true;
      break;
    }
    i++;
  }

  if (!found)
  {
    //		char *id_str;
    //
    //		long int m = 1000;
    //		snprintf(id_str, m, "%i", i);
    //
    //		Slice key = Slice(id_str);

    leveldb::Status s = db->Put(leveldb::WriteOptions(), "0", value);

    assert(s.ok());
  }

  assert(it->status().ok());

  delete it;

  return 0;
}

int lvldb_database_del(lua_State *L)
{
  leveldb::DB *db = check_database(L, 1);

  leveldb::Slice key = lua_to_slice(L, 2);

  leveldb::Status s = db->Delete(lvldb_wopt(L, 3), key);

  if (s.ok())
  {
    lua_pushboolean(L, true);
    return 1;
  }
  else
  {
    //std::cerr << "Error deleting key/value entry: " << s.ToString() << std::endl;
    lua_pushboolean(L, false);
    string_to_lua(L, s.ToString());
  }

  return 2;
}

/**
 * LevelDB iterator functions
 * --------------------------
 */

/**
 * Method that creates an iterator.
 */
int lvldb_database_iterator(lua_State *L)
{
  leveldb::DB *db = check_database(L, 1);

  leveldb::Iterator *it = db->NewIterator(lvldb_ropt(L, 2));
  leveldb::Iterator **ptr = (leveldb::Iterator **)lua_newuserdata(L, sizeof(void *));
  ptr[0] = it;

  luaL_getmetatable(L, LVLDB_MT_ITER);
  lua_setmetatable(L, -2);

  return 1;
}

//TODO test it
int lvldb_database_write(lua_State *L)
{
  leveldb::DB *db = check_database(L, 1);

  leveldb::Status s = db->Write(lvldb_wopt(L, 3), check_writebatch(L, 2));

  if (s.ok())
  {
    lua_pushboolean(L, true);
    return 1;
  }
  else
  {
    //std::cerr << "Error getting value (get): " << s.ToString() << std::endl;
    lua_pushboolean(L, false);
    string_to_lua(L, s.ToString());
  }
  return 2;
}

/**
 * To string function for a DB.
 * ----------------------------
 * Not to use in production environments.
 * Just prints the whole set of key,values from a DB.
 */
int lvldb_database_tostring(lua_State *L)
{
  leveldb::DB *db = check_database(L, 1);
  std::ostringstream oss(std::ostringstream::out);

  leveldb::Iterator *it = db->NewIterator(leveldb::ReadOptions());

  oss << "DB output:" << std::endl;
  it->SeekToFirst();

  if (!it->Valid())
    oss << "Database is empty." << std::endl;
  else
  {
    //for (it->SeekToFirst(); it->Valid(); it->Next()) {
    while (it->Valid())
    {
      oss << it->key().ToString() << " -> " << it->value().ToString() << std::endl;

#ifdef LUALEVELDB_LOGMODE
      //std::cout << "LOG: " << it->key().ToString() << " -> "  << it->value().ToString() << std::endl;
#endif

      it->Next();
    }
  }

  assert(it->status().ok());

  delete it;

  lua_pushstring(L, oss.str().c_str());

  return 1;
}
