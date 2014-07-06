#!/bin/bash

# Author: Marco Pompili - marcs.pompili@gmail.com
# Script file for removing lua-leveldb from the installed rocks.

LUAROCKS="luarocks"

sudo $LUAROCKS remove lua-leveldb

sudo make clean
