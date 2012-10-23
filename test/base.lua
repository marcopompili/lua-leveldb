package.cpath = package.cpath .. ';../Debug/?.so'

require 'lua-leveldb'

local test_key = 'key1'
local test_val = 'value1'

print ('opening test.db')
local testdb = leveldb.open ('test.db')

if leveldb.check(testdb)
then

	if testdb:put(test_key, test_val)
	then
		print ("Getting test " .. test_key .. " : " .. testdb:get(test_key))
	end
end

--print ('closing test.db')
--close ('test.db')