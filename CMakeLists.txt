cmake_minimum_required(VERSION 2.6)
project(lualeveldb DESCRIPTION "Lua bindings for google's leveldb library.")
set(lualeveldb_VERSION_MAJOR 0)
set(lualeveldb_VERSION_MINOR 4)


add_library(lualeveldb SHARED
  src/llua.hpp
  src/lvllib.hpp
  src/lutils.h
  src/lutils.c
  src/batch.hpp
  src/batch.cc
  src/db.hpp
  src/db.cc
  src/iter.hpp
  src/iter.cc
  src/opt.hpp
  src/opt.cc
  src/utils.hpp
  src/utils.cc
  src/lua-leveldb.hpp
  src/lua-leveldb.cc)

set_target_properties(lualeveldb PROPERTIES VERSION 0.4)
set_target_properties(lualeveldb PROPERTIES SOVERSION 1)

include(GNUInstallDirs)

install(TARGETS lualeveldb
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
