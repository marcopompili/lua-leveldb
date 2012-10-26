package.cpath = package.cpath .. ';../Debug/?.so'

require 'lua-leveldb'

local db = leveldb.open('batch.db')
local bt = db:batch()

bt:put('key1','value')