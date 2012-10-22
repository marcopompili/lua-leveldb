package.cpath = package.cpath .. ';../Debug/?.so'

local leveldb = require 'lua-leveldb'

version ()

open ('test.db')

--close ('test.db')