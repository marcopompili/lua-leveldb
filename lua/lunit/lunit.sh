#!/bin/bash

LUNIT="lunit.sh -i /usr/bin/lua5.1"

$LUNIT basic.lua
$LUNIT iterator.lua
$LUNIT tostring.lua
