#!/bin/bash

# Author: Marco Pompili - marcs.pompili@gmail.com
# Script file for removing lua-leveldb from the installed rocks.

LUAROCKS51="/usr/bin/luarocks-5.1"

if [ -f $LUAROCKS51 ]
then
	LUAROCKS="luarocks-5.1"
else
	LUAROCKS="luarocks"
fi

sudo $LUAROCKS remove lua-leveldb

sudo make clean
