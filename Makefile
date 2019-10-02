CC=gcc
CXX=g++
CFLAGS=-Wall -fPIC -shared
CXXFLAGS=-Wall -fPIC -shared
VPATH=src
LDLIBS=-lleveldb -lsnappy -lpthread
CDEPS=src/*.h
CXXDEPS=src/*.hpp
OBJS=src/lua.o src/batch.o src/db.o src/iter.o src/opt.o src/utils.o src/lua-leveldb.o
RM=rm -f
TARGET=lualeveldb.so

# Add a custom version below (5.1/5.2/5.3)
LUA_VERSION?=5.3
LUA_PREFIX_DIR?=/usr
LUA_LIBDIR=$(LUA_PREFIX_DIR)/lib/lua/$(LUA_VERSION)
LUA_INCDIR=$(LUA_PREFIX_DIR)/include/lua$(LUA_VERSION)
LUA_SHAREDIR=$(LUA_PREFIX_DIR)/share/lua/$(LUA_VERSION)


$(TARGET): $(OBJS)
	@echo -e '\033[0;36mBuilding target $@\033[0m'
ifneq ($(wildcard $(LUA_LIBDIR)),)
	$(CXX) $(CXXFLAGS) -o $(TARGET) -L$(LUA_LIBDIR) $(OBJS) $(LDLIBS)
else
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)
endif

%.o: %.c $(CDEPS)
ifneq ($(wildcard $(LUA_INCDIR)),)
	$(CC) -c -o $@ $< $(CFLAGS) -I$(LUA_INCDIR)
else
	$(CC) -c -o $@ $< $(CFLAGS)
endif

%.o: %.cc $(CXXDEPS)
ifneq ($(wildcard $(LUA_INCDIR)),)
	$(CXX) -c -o $@ $< $(CXXFLAGS) -I$(LUA_INCDIR)
else
	$(CXX) -c -o $@ $< $(CXXFLAGS)
endif

liblualeveldb.o: lua.h batch.hpp db.hpp iter.hpp opt.hpp utils.hpp lua-leveldb.hpp
ifneq ($(wildcard $(LUA_INCDIR)),)
	$(CXX) $(CXXFLAGS) -o -I$(LUA_INCDIR)
else
	$(CXX) $(CXXFLAGS) -o
endif

.PHONY: clean
clean:
	$(RM) $(OBJS)
	$(RM) $(TARGET)

install:
	@echo -e '\033[0;32mInstalling with luarocks\033[0m'
