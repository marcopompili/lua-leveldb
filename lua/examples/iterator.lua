local leveldb = require 'leveldb'

local opt = leveldb.options()
opt.createIfMissing = true
opt.errorIfExists = false

-- db example
local db = leveldb.open(opt, 'iterator.db')

db:put('key1', 'value1')
db:put('key1n', 1)
db:put('key2', 'value2')
db:put('key2n', 2)
db:put('key3', 'value3')
db:put('key3n', 3.14)

local iter = db:iterator()

iter:seekToFirst()

while(iter:valid())
do
    print(iter:key() .. ' ' .. iter:value()) -- value() or string() can be used

    iter:next()
end

iter:del()

leveldb.close(db)