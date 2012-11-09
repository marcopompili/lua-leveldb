CXX = g++
CFLAGS =
CXXFLAGS = -Wall -fPIC
LIBFLAG = -shared
LDFLAGS = -lleveldb -lsnappy -lpthread

LUA_DIR=/usr/local
LUA_LIBDIR=$(LUA_DIR)/lib/lua/5.1
LUA_INCDIR=/usr/include/lua5.1
LUA_SHAREDIR=$(LUA_DIR)/share/lua/5.1

all: leveldb.so

leveldb.so:
	@echo -e '\033[0;34mBuilding target: $@\033[0m'
	$(CXX) src/lua-leveldb.cc $(CXXFLAGS) $(LIBFLAG) -I$(LUA_INCDIR) $(LDFLAGS) -o leveldb.so

clean:
	@echo -e '\033[0;33mCleaning binaries:\033[0m'
	rm -f src/config.h
	rm -f leveldb.so

install:
	@echo -e '\033[0;32mInstalling with luarocks\033[0m'
