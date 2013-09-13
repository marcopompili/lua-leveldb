require 'leveldb'

local opt = leveldb.options()
opt.createIfMissing = true
opt.errorIfExists = false

local db = leveldb.open(opt, 'test.db')

db:put('key1', 'value1')
db:put('key2', 'value2')
db:put('key3', 'value3')

local iter = db:iterator()

iter:seekToFirst()

while(iter:valid())
do
    print(iter:key() .. ' ' .. iter:value())

    iter:next() -- go next
end