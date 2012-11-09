CC = g++
RM = rm -rf
LIBFLAG = -shared

LIBLEVELDB_DIR = /usr/local
LIBLEVELDB_INCDIR = $(LIBLEVELDB_DIR)/include
LIBLEVELDB_LIBDIR = $(LIBLEVELDB_DIR)/lib

LUA_DIR = /usr/lib
LUA_LIBDIR = $(LUA_DIR)/lua/5.1
LUA_INCDIR = /usr/include/lua5.1

all: leveldb.so

leveldb.so:
	@echo 'Building target: $@'
	$(CC) -Wall ${LIBFLAG} -fPIC -I${LUA_INCDIR} -lleveldb -lsnappy -lpthread -o leveldb.so src/lua-leveldb.cc
	
clean:
	@echo 'Cleaning binaries:'
	rm src/config.h
	rm leveldb.so
	
install:
	@echo 'Installing from luarocks'