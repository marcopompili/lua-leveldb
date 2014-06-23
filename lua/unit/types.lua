require 'leveldb'

opt = leveldb.options()
opt.createIfMissing = true
opt.errorIfExists = false

db = leveldb.open(opt, 'types.db')
assert(db:put("string", "Oh hai Mark!"))
assert(db:put("number", 123456))

assert(type(db:get("string")) == "string")
assert(type(db:get("number")) == "number")

leveldb.close(db)