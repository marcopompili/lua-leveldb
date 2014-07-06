local leveldb = require 'leveldb'

opt = leveldb.options()
opt.createIfMissing = true
opt.errorIfExists = false

db = leveldb.open(opt, 'types.db')
assert(db:put("string", "Oh hai Mark!"))
assert(db:put("number", 123456))
assert(db:put("boolean", true))

function check(val, expected)
	assert(type(val) == expected, "expected " .. expected .. ", found: " .. type(val))
end

check(db:get("string"), "string")
check(db:get("number"), "number")
check(db:get("boolean"), "boolean")

print("all type tests are fine!")

leveldb.close(db)