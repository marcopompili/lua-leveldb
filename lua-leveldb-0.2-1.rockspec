package = "lua-leveldb"
version = "0.2-1"
source = {
   url = "git://github.com/marcopompili/lua-leveldb.git",
   branch = "master"
}
description = {
   summary = "LevelDB extension for Lua.",
   homepage = "https://github.com/marcopompili/lua-leveldb",
   license = "MIT/X11",
   maintainer = "Marco Pompili <marcs.pompili@gmail.com>"
}
dependencies = {
   "lua >= 5.1"
}
build = {
   type = "make",
   modules = {
      leveldb = "src/lua-leveldb.cc",
   }
}