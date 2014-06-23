local leveldb = require 'leveldb'

local opt = leveldb.options()
opt.createIfMissing = true
opt.errorIfExists = false

-- string example
local db_str = leveldb.open(opt, 'str.db')

assert(db_str:put('key1', 'value1'))
assert(db_str:put('key2', 'value2'))
assert(db_str:put('key3', 'value3'))

local iter = db_str:iterator()

iter:seekToFirst()

while(iter:valid())
do
    print(iter:key() .. ' ' .. iter:value()) -- value() or string() can be used

    iter:next()
end

iter:del()

leveldb.close(db_str)

-- number example
local db_num = leveldb.open(opt, 'num.db')

assert(db_num:put('key1', 1))
assert(db_num:put('key2', 2))
assert(db_num:put('key3', 3.14))

iter = db_num:iterator()

iter:seekToFirst()

while(iter:valid())
do
	print(iter:key() .. ' ' .. iter:valnum()) -- get value as number
	
	iter:next()
end

iter:del()

leveldb.close(db_num)