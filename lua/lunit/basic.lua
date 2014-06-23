require 'lunit'
local leveldb = require 'leveldb'


module("resource_testcase", lunit.testcase, package.seeall)


function setup()
	opt = leveldb.options()
	opt.createIfMissing = true
	opt.errorIfExists = false
	
	testdb = leveldb.open(opt, 'test.db')
end


function teardown()
	leveldb.close(testdb)
end


function test_put()
	test_key = 'key'
	local test_val = 'value'

	assert_true(testdb:put(test_key, test_val), 'Put operation went no good')
end


function test_get()
	assert_true(leveldb.check(testdb), 'DB is broken')
	assert_not_nil(testdb:get(test_key), 'TestKey (' .. test_key .. ') value is nil')
end