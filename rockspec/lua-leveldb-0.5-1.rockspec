package = "lua-leveldb"
version = "0.5-1"
source = {
	url = "git://github.com/marcopompili/lua-leveldb.git",
	tag = "0.5"
}
description = {
	summary = "LevelDB bindings for Lua.",
	detailed = [[
		LevelDB is a fast key-value storage library written 
		at Google that provides an ordered mapping from string 
		keys to string values. This is an extension to use
		LevelDB directly from Lua. It has been tested on Lua 5.1/5.2/5.3.
	]],
	homepage = "https://github.com/marcopompili/lua-leveldb",
	license = "MIT/X11",
	maintainer = "Marco Pompili <pompilimrc@gmail.com>"
}
dependencies = {
	"lua >= 5.1"
}
external_dependencies = {
	LEVELDB_DB = {
		header = "leveldb/db.h"
	},
	LEVELDB_STATUS = {
		header = "leveldb/status.h"
	},
	LEVELDB_OPTIONS = {
		header = "leveldb/options.h"
	},
	LEVELDB_WRITE_BATCH = {
		header = "leveldb/write_batch.h"
	},
	LEVELDB_FILTER_POLICY = {
		header = "leveldb/filter_policy.h"
	}
}
build = {
	type = "make",
	install = {
		lib = {
			"lualeveldb.so"
		}
	},
	modules = {
		leveldb = "src/lua-leveldb.cc",
		incdirs = {
			"$(LEVELDB_DB_INCDIR)",
			"$(LEVELDB_STATUS_INCDIR)",
			"$(LEVELDB_OPTIONS_INCDIR)",
			"$(LEVELDB_WRITE_BATCH_INCDIR)",
			"$(LEVELDB_FILTER_POLICY_INCDIR)"
		},
		libdirs = {
			"$(LEVELDB_DB_LIBDIR)",
			"$(LEVELDB_STATUS_LIBDIR)",
			"$(LEVELDB_OPTIONS_LIBDIR)",
			"$(LEVELDB_WRITE_BATCH_LIBDIR)",
			"$(LEVELDB_FILTER_POLICY_LIBDIR)"
		}
	},
	copy_directories = { "doc" }
}
