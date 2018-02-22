#!/bin/bash

LUAROCKS="luarocks"

$LUAROCKS --local remove lua-leveldb

make clean
