leveldb = require 'liblualeveldb'

opt = leveldb.options()
opt.createIfMissing = true
opt.errorIfExists = false

-- setting the bloom filter into the options
leveldb.bloomFilterPolicy(opt, 10)

print(opt)

local test_key = 'key1'
local test_val = 'value1'

print ('opening test.db')
testdb = leveldb.open(opt, 'filter.db')

if leveldb.check(testdb)
then
    if testdb:put(test_key, test_val)
    then
        print ('key1: '.. testdb:get(test_key))
    end
end

leveldb.close(testdb)

testdb = leveldb.open(opt, 'filter.db')
testdb:put('key2', 123456)

print ('key2: ' .. testdb:get('key2'))

leveldb.close(testdb)
