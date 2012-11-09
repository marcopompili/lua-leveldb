#!/bin/bash

set -e

LEVELDB_INCLUDE_PATH=/usr/include/leveldb
CONFIG_HEADER=src/config.h
VERSION=0.2
REVISION=1

if [ -f "$CONFIG_HEADER" ]
then
	rm -f $CONFIG_HEADER
fi

echo -e "\033[0;32mlua-leveldb $VERSION-$REVISION\033[0m"
echo "Configuring:"
echo -ne "  *filter_policy.h header found: "

if [ -f "$LEVELDB_INCLUDE_PATH/filter_policy.h" ]
then
	echo -e "\033[0;32myes\033[0m"
	echo "#define LEVELDB_FILTER_POLICY_H 1" >> $CONFIG_HEADER
else
	echo -e "\033[0;31mno\033[0m"
fi

g++ -Wall -shared -fPIC -llua -lleveldb -lsnappy -lpthread -o leveldb.so src/lua-leveldb.cc

luarocks make rockspec/lua-leveldb-$VERSION-$REVISION.rockspec
