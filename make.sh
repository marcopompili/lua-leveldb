#!/bin/bash

# Author: Marco Pompili - marcs.pompili@gmail.com
# Script file for installing the dev version of lua-leveldb.

set -e

LEVELDB_INCLUDE_PATH=/usr/include/leveldb
CONFIG_HEADER=src/config.h
VERSION=0.2
REVISION=1

LUAROCKS51="/usr/bin/luarocks-5.1"
LUAROCKS52="/usr/bin/luarocks-5.2"

echo -e "\e[1:32mChecking lua version:\e[0m"

if [ -f "$LUAROCKS51" ]
then
	echo "Found specific lua 5.1 version"
	LUAROCKS="luarocks-5.1"
else
	echo "Warning found general lua version"
	LUAROCKS="luarocks"
fi


if [ -f "$CONFIG_HEADER" ]
then
	rm -f $CONFIG_HEADER
fi

touch $CONFIG_HEADER

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

$LUAROCKS make rockspec/lua-leveldb-$VERSION-$REVISION.rockspec
