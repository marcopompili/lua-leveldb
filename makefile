CXX = g++
CFLAGS = 
CXXFLAGS = -Wall -fPIC
LIBFLAG = -shared
LDFLAGS = -lleveldb -lsnappy -lpthread

LUA_DIR=/usr/local
LUA_LIBDIR=$(LUA_DIR)/lib/lua/5.2
LUA_INCDIR=/usr/include/lua5.2
LUA_SHAREDIR=$(LUA_DIR)/share/lua/5.2

all: leveldb.so

leveldb.so:
	@echo -e '\033[0;34mBuilding target: $@\033[0m'
ifneq (,$(wildcard /usr/include/leveldb/filter_policy.h))
	$(CXX) src/lua-leveldb.cc src/lvldb-serializer.cpp $(CXXFLAGS) $(LIBFLAG) -I$(LUA_INCDIR) $(LDFLAGS) -o leveldb.so -DLEVELDB_FILTER_POLICY_H=1
else
	$(CXX) src/lua-leveldb.cc src/lvldb-serializer.cpp $(CXXFLAGS) $(LIBFLAG) -I$(LUA_INCDIR) $(LDFLAGS) -o leveldb.so
endif

clean:
	@echo -e '\033[0;33mCleaning binaries:\033[0m'
	rm -f leveldb.so
	
install:
	@echo -e '\033[0;32mInstalling with luarocks\033[0m'