lua-leveldb
===========

Leveldb extension for Lua.

From the googlecode page:
<< The leveldb library provides a persistent key value store. Keys and values are arbitrary byte arrays.
The keys are ordered within the key value store according to a user-specified comparator function. >>

I wrote an extension because i needed a fast embeddable key/value database to use it with Wireshark.
I needed to analyze some big pcap files and to divide the traffic using some given rules. So i wrote
this basic lua extension to access leveldb functions directly from lua.

Usage
-----
This is a simple example on how to use the lua extension for Google's leveldb:

	package.cpath = package.cpath .. ';./lib/?.so'

	require 'leveldb'

	local opt = leveldb.options();
	opt.createIfMissing = true
	opt.errorIfExists = false

	local ropt = leveldb.readOptions();
	local wopt = leveldb.writeOptions();

	local test_key = 'key1'
	local test_val = 'value1'

	print ('opening test.db')
	local testdb = leveldb.open (opt, 'test.db')

	if leveldb.check(testdb)
	then
		if testdb:put(wopt, test_key, test_val)
		then
			print ("Getting test " .. test_key .. " : " .. testdb:get(ropt, test_key))
		end
	end

	leveldb.close(testdb)