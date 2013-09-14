require 'leveldb'

local opt = leveldb.options()
opt.createIfMissing = true
opt.errorIfExists = false

print(opt)

local db = leveldb.open (opt, 'test.db')

print(db)