require 'lunit'
local leveldb = require 'leveldb'


module("resource_testcase", lunit.testcase, package.seeall)


function setup()
	opt = leveldb.options()
	opt.createIfMissing = true
	opt.errorIfExists = false

	db = leveldb.open (opt, 'test.db')
end


function teardown()
	leveldb.close(db)
end


function test_tostring()
	assert_string(tostring(opt))
	assert_string(tostring(db))
end