package.cpath = package.cpath .. ';../Debug/?.so'

local leveldb = require 'lua-leveldb'

local test_key = 'key1'
local test_val = 'value1'

print ('opening test.db')
local testdb = leveldb.open ('test.db')

leveldb.check(testdb)

testdb:put(test_key, test_val)
print (testdb:get(test_key))

--print ('try to write a record: ' .. test_key)
--if put (test_key, test_val)
--then
	--print('record ' .. test_key .. ' successful')
--end 

--print ('try to get a record: ' .. test_key .. ' = ' .. get (test_key))

--print ('closing test.db')
--close ('test.db')