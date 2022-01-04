#include "db.hpp"

/**
 * Data Ops
 * ----------------------------------------------------------------
 * Data operations are binded to a db instance, the first parameter
 * is always a db, the notation used in Lua is db:put, db:get etc.
 */

/**
 * Method that puts a key,value pair using LevelDB Slice format.
 * -------------------------------------------------------------$
 * This method returns:
 *   * True on success
 *   * luaL_error on error
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
    luaL_error(L, "db:put call error, %s\"%s\"\n", s.ToString().c_str(), key.ToString().c_str());

    return -1;
  }
}

/**
 * Method that gets a value by key.
 * --------------------------------$
 * This method returns:
 *  * String on success
 *  * nil on missing key
 *  * luaL_error call on error
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
  }
  else if (s.IsNotFound())
  {
    lua_pushnil(L);
  }
  else
  {
    luaL_error(L, "db:get call error, %s\"%s\"\n", s.ToString().c_str(), key.ToString().c_str());

    return -1;
  }

  return 1;
}

/**
 * Method that checks if key exists.
 * ---------------------------------$
 * This method returns:
 *  * True if key is found
 *  * False if key is not found
 *  * luaL_error on error
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
  else if (s.IsNotFound())
  {
    lua_pushboolean(L, false);
  }
  else
  {
    luaL_error(L, "db:has call error, %s%s\n", s.ToString().c_str(), key.ToString().c_str());

    return -1;
  }

  return 1;
}

/*
 * Method for deleting a key,value.
 * --------------------------------$
 * This method returns:
 *  * True on success
 *  * False on missing key
 *  * luaL_error on error
 */
int lvldb_database_del(lua_State *L)
{
  leveldb::DB *db = check_database(L, 1);

  leveldb::Slice key = lua_to_slice(L, 2);

  leveldb::Status s = db->Delete(lvldb_wopt(L, 3), key);

  if (s.ok())
  {
    lua_pushboolean(L, true);
  }
  else if (s.IsNotFound())
  {
    lua_pushboolean(L, false);
  }
  else
  {
    luaL_error(L, "db:del call error, %s\"%s\"\n", s.ToString().c_str(), key.ToString().c_str());

    return -1;
  }

  return 1;
}

/**
 * LevelDB iterator functions
 * --------------------------
 */

/**
 * Method that creates an iterator.
 * --------------------------------$
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


/*
 * Method for writing an atomic transaction.
 * -----------------------------------------$
 * This method returns:
 * * True on success
 * * luaL_error on error
 */
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
    luaL_error(L, "db:write call error, %s\n", s.ToString().c_str());

    return -1;
  }
}


/**
 * To string function for db.
 * --------------------------$
 * Not to use in production, dev only.
 * Prints the whole set of key, values of a db.
 */
int lvldb_database_tostring(lua_State *L)
{
  leveldb::DB *db = check_database(L, 1);

  std::ostringstream oss(std::ostringstream::out);

  leveldb::Iterator *it = db->NewIterator(leveldb::ReadOptions());

  oss << "DB output:" << std::endl;

  it->SeekToFirst();

  if (!it->Valid())
  {
    oss << "Database is empty." << std::endl;
  }
  else
  {
    while (it->Valid())
    {
      oss << it->key().ToString() << " -> " << "\"" << it->value().ToString() << "\"" << std::endl;

      it->Next();
    }
  }

  if (it->status().ok())
  {
    lua_pushstring(L, oss.str().c_str());

    delete it;

    return 1;
  }
  else
  {
    luaL_error(L, "db tostring error, %s\n", it->status().ToString().c_str());

    delete it;

    return -1;
  }
}
