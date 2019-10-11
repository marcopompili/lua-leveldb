leveldb = require 'lualeveldb'

opt = leveldb.options()
opt.createIfMissing = true
opt.errorIfExists = false

local db_file = 'basic.db'
local test_key = 'key1'
local test_val = 'val1'

print ('opening ' .. db_file .. '...')
local db = leveldb.open(opt, db_file)

if leveldb.check(db)
then
    if db:put(test_key, test_val)
    then
        print ('key1: '.. db:get(test_key))
    end
end

leveldb.close(db)

db = leveldb.open(opt, db_file)
db:put('key2', '123456')

print ('key2: ' .. db:get('key2'))

leveldb.close(db)
