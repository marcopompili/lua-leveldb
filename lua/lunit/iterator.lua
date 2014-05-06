require 'lunit'
require 'leveldb'


module("resource_testcase", lunit.testcase, package.seeall)


function setup()
	opt = leveldb.options()
	opt.createIfMissing = true
	opt.errorIfExists = false
end


function teardown()
	leveldb.close(db)
end


function test_string()
	db = leveldb.open(opt, 'str.db')

	db:put('key1', 'value1')
	db:put('key2', 'value2')
	db:put('key3', 'value3')

	local iter = db:iterator()

	iter:seekToFirst()

	while(iter:valid())
	do
		assert_not_nil(iter:key())
		assert_string(iter:value()) -- value() or string() can be used

		iter:next()
	end

	iter:del()
end


function test_number()
	db = leveldb.open(opt, 'num.db')

	db:put('key1', 1)
	db:put('key2', 2)
	db:put('key3', 3.14)

	local iter = db:iterator()

	iter:seekToFirst()

	while(iter:valid())
	do
		assert_not_nil(iter:key())
		assert_number(iter:valnum()) -- get value as number

		iter:next()
	end

	iter:del()
end