leveldb = require 'lualeveldb'

local opt = leveldb.options()
opt.createIfMissing = true
opt.errorIfExists = false

-- db example
local db = leveldb.open(opt, 'iterator.db')

assert(db:put('key1s', 'value1'))
assert(db:put('key1n', 1))
assert(db:put('key2s', 'value2'))
assert(db:put('key2n', 2))
assert(db:put('key3s', 'value3'))
assert(db:put('key3n', 3.14))

local iter = db:iterator()

iter:seekToFirst()

while(iter:valid())
do
   local key = iter:key()
   local value = iter:value();
   print(iter:key() .. ' => ' .. '(' .. type(value) .. ') ' .. tostring(value))

   iter:next()
end

iter:del()

leveldb.close(db)
