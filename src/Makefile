CC=gcc
CXX=g++
CFLAGS=-Wall -fPIC -shared
CXXFLAGS=-Wall -fPIC -shared
LDLIBS=-lleveldb -lsnappy -lpthread
OBJS=lua.o batch.o db.o iter.o opt.o utils.o lua-leveldb.o
RM=rm -f
TARGET=liblualeveldb.so

LUA_VERSION=5.3
LUA_DIR=/usr/local
LUA_LIBDIR=$(LUA_DIR)/lib/lua/$(LUA_VERSION)
LUA_INCDIR=/usr/include/lua$(LUA_VERSION)
LUA_SHAREDIR=$(LUA_DIR)/share/lua/$(LUA_VERSION)


$(TARGET): $(OBJS)
	@echo -e '\033[0;36mBuilding target $@\033[0m'
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

liblualeveldb.o: lua.h batch.hpp db.hpp iter.hpp opt.hpp utils.hpp lua-leveldb.hpp
	$(CXX) -c -o $(CXXFLAGS) $(LDLIBS)

.PHONY: clean
clean:
	$(RM) $(OBJS)
	$(RM) $(TARGET)

install:
	@echo -e '\033[0;32mInstalling with luarocks\033[0m'
