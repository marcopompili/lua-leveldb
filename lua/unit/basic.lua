local leveldb = require 'leveldb'

opt = leveldb.options()
opt.createIfMissing = true
opt.errorIfExists = false

local test_key = 'key1'
local test_val = 'value1'

-- first run
print ('opening test.db')

testdb = leveldb.open(opt, 'test.db')

assert(leveldb.check(testdb), "inconsistent db")

assert(testdb:put(test_key, test_val), "cannot put k/v record")

assert(testdb:get(test_key) == test_val, "value inconsistent")

leveldb.close(testdb)

-- reopening
print ('reopening test.db')

testdb = leveldb.open(opt, 'test.db')

assert(testdb:put('key2', 123456), "cannot put second k/v record")

local return_val = testdb:get('key2')

assert(return_val == 123456, "second value is inconsistent, expecting number, found: " .. type(return_val))

leveldb.close(testdb)