CXX = g++
CFLAGS = 
CXXFLAGS = -Wall -fPIC
LIBFLAG = -shared
LDFLAGS = -lleveldb -lsnappy -lpthread

LUA_VERSION=5.1
LUA_DIR=/usr/local
LUA_LIBDIR=$(LUA_DIR)/lib/lua/$(LUA_VERSION)
LUA_INCDIR=/usr/include/lua$(LUA_VERSION)
LUA_SHAREDIR=$(LUA_DIR)/share/lua/$(LUA_VERSION)
TARGET=liblua-leveldb.so

all: $(TARGET)

$(TARGET): src/lua-leveldb.cc
	@echo -e '\033[0;34mBuilding target: $@\033[0m'
ifneq (,$(wildcard /usr/include/leveldb/filter_policy.h))
	$(CXX) src/lua-leveldb.cc src/lvldb-serializer.cpp $(CXXFLAGS) $(LIBFLAG) -I$(LUA_INCDIR) $(LDFLAGS) -o $(TARGET) -DLEVELDB_FILTER_POLICY_H=1
else
	$(CXX) src/lua-leveldb.cc src/lvldb-serializer.cpp $(CXXFLAGS) $(LIBFLAG) -I$(LUA_INCDIR) $(LDFLAGS) -o $(TARGET)
endif

clean:
	@echo -e '\033[0;33mCleaning binaries:\033[0m'
	rm -f $(TARGET)

install:
	@echo -e '\033[0;32mInstalling with luarocks\033[0m'
