CC = g++
RM = rm -rf
LIBFLAG = -shared

LIBLEVELDB_DIR = /usr/local
LIBLEVELDB_INCDIR = $(LIBLEVELDB_DIR)/include
LIBLEVELDB_LIBDIR = $(LIBLEVELDB_DIR)/lib

LUA_DIR = /usr/lib
LUA_LIBDIR = $(LUA_DIR)/lua/5.1

all: leveldb.so

leveldb.so:
	@echo 'Building target: $@'
	mkdir -p bin
	$(CC) -Wall ${LIBFLAG} -fPIC -lleveldb -o "bin/leveldb.so" src/lua-leveldb.cc
	
clean:
	@echo 'Cleaning bin...'
	$(RM) bin
	
install:
	mkdir -p ${LUA_LIBDIR}
	cp bin/leveldb.so ${LUA_LIBDIR}
	
uninstall:
	rm ${LUA_LIBDIR}/leveldb.so