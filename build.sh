#!/bin/bash

# Author: Marco Pompili - pompilimrc@gmail.com
# Script file for installing the dev version of lua-leveldb.

if [ $# -lt 2 ]
then
	echo -e "\e[1;31mUSAGE: $0 <version> <revision> \e[0;31m[luarocks-path]\e[0m"
	echo -e "\e[0;32m[luarocks-path] is optional, just use it if you want to use a specific version of Lua.\e[0m"
	exit 1
fi

if [ $# -gt 2 ]
then
	LUAROCKS=$3
else
	LUAROCKS=luarocks
fi

set -e

VERSION=$1
REVISION=$2

echo -e "Going for version $1 and revision $2..."

echo -e "\033[0;32mlua-leveldb $VERSION-$REVISION\033[0m"

$LUAROCKS --local make rockspec/lua-leveldb-$VERSION-$REVISION.rockspec
