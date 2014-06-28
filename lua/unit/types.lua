local leveldb = require 'leveldb'

opt = leveldb.options()
opt.createIfMissing = true
opt.errorIfExists = false

db = leveldb.open(opt, 'types.db')
assert(db:put("string", "Oh hai Mark!"))
assert(db:put("number", 123456))

function check(val, expected)
	assert(type(val) == expected, "expected " .. expected .. ", found: " .. type(val))
	print ("output: " .. val)
end

check(db:get("string"), "string")
check(db:get("number"), "number")

leveldb.close(db)