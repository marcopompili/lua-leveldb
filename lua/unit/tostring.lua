local leveldb = require 'leveldb'

local opt = leveldb.options()
opt.createIfMissing = true
opt.errorIfExists = false

assert(type(tostring(opt)) == "string")
print(opt)

local db = leveldb.open (opt, 'test.db')

assert(type(tostring(db)) == "string")
print(db)