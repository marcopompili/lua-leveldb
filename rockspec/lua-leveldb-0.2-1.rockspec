package = "lua-leveldb"
version = "0.2-1"
source = {
	url = "git://github.com/marcopompili/lua-leveldb.git",
	branch = "0.2",
	md5 = "fa05ed5865e02b7fd412c7ec43b20d85"
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
	install = {lib = {"leveldb.so"}},
	modules = {
		leveldb = "src/lua-leveldb.cc",
	}
}
