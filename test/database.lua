package.cpath = package.cpath .. ';../Debug/?.so'

require 'lua-leveldb'

local test_key = 'key1'
local test_val = 'value1'

local db = leveldb.open('database.db')

if leveldb.check(db)
then

	if db:put(test_key, test_val)
	then
		print("Getting test " .. test_key .. " : " .. db:get(test_key))
	end
end

leveldb.close('database.db')