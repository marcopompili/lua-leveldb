#!/bin/bash

# Author: Marco Pompili - marcs.pompili@gmail.com
# Script file for installing the dev version of lua-leveldb.

if [ $# -ne 2 ]
then
	echo -e "\e[1;31mUSAGE: $0 <version> <revision>\e[0m"
	exit 1
fi

set -e

LEVELDB_INCLUDE_PATH=/usr/include/leveldb
CONFIG_HEADER=src/config.h
VERSION=$1
REVISION=$2

echo -e "Going for version $1 and revision $2..."

LUAROCKS51="/usr/bin/luarocks-5.1"
LUAROCKS52="/usr/bin/luarocks-5.2"

echo -e "\e[1:32mChecking lua version:\e[0m"

if [ -f "$LUAROCKS51" ]
then
	echo "Found specific lua 5.1 version"
	LUAROCKS="luarocks-5.1"
elif [ -f "$LUAROCKS52" ]
then
	echo "Found specific lua 5.2 version"
	LUAROCKS="luarocks-5.2"
else
	echo -e "Warning found general lua version"
	LUAROCKS="luarocks"
fi

echo -e "\033[0;32mlua-leveldb $VERSION-$REVISION\033[0m"
echo "Configuring:"
echo -ne "  *filter_policy.h header found: "

if [ -f "$LEVELDB_INCLUDE_PATH/filter_policy.h" ]
then
	echo -e "\033[0;32myes\033[0m"
else
	echo -e "\033[0;31mno\033[0m"
fi

$LUAROCKS make rockspec/lua-leveldb-$VERSION-$REVISION.rockspec