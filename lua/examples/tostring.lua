leveldb = require 'liblualeveldb'

local opt = leveldb.options()
opt.createIfMissing = true
opt.errorIfExists = false

assert(type(tostring(opt)) == "string")
print(opt)

local db = leveldb.open (opt, 'tostring.db')

assert(type(tostring(db)) == "string")
print(db)
