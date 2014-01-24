lua-leveldb
===========

Lua bindings for google's leveldb key/store database.

From the googlecode page:
"The leveldb library provides a persistent key value store. Keys and values are arbitrary byte arrays.
The keys are ordered within the key value store according to a user-specified comparator function."

I wrote an extension because i needed a fast embeddable key/value database to use it with WireShark for packet analysis.
I needed to analyze some big pcap files and divide the traffic using some given rules.
So i wrote this basic lua extension to access Leveldb functions directly from lua that's used as extension language for WireShark.

Most of the basic options and functions are supported right now, but still not the full set of operations permitted by Leveldb.

Installation
------------
The library is packed like a luarock extension, check out luarocks (http://luarocks.org/).
  * Before installing check that you got leveldb correctly installed for your linux distribution.
  * To install execute make.sh as root, this script should build the library and install it as a luarock package.
  * To remove the library use remove.sh (as root) to remove the package and delete the built files.

Support
-------
The extension still not support the full set of operations permitted by the leveldb library.

This is what i got working for now:
  * Basic open/close/repair database operations supported.  
  * Basic database write, read and delete operations.
  * Partial access to most important options settings is supported.
  * Iterators with seek and iteration operations are supported.
  * Atomic batch updates supported.
  * Experimental support for unique values.
  * Added support for storing numeric values.
  * Multiple methods for getting values (as string or number).
  * ToString support for varius objects.

Basic Example
-------------
This is a simple example on how to use the lua extension for Google's leveldb:

```lua
require 'leveldb'

local opt = leveldb.options()
opt.createIfMissing = true
opt.errorIfExists = false

local test_key = 'key1'
local test_val = 'value1'

print ('opening test.db')
local testdb = leveldb.open (opt, 'test.db')

if leveldb.check(testdb)
then
	if testdb:put(test_key, test_val)
	then
		print ("Getting test " .. test_key .. " : " .. testdb:get(test_key))
	end
end

leveldb.close(testdb)
```

Iterator Example
----------------
```lua
require 'leveldb'

local opt = leveldb.options()
opt.createIfMissing = true
opt.errorIfExists = false

-- string example
local db_str = leveldb.open(opt, 'str.db')

db_str:put('key1', 'value1')
db_str:put('key2', 'value2')
db_str:put('key3', 'value3')

local iter = db_str:iterator()

iter:seekToFirst()

while(iter:valid())
do
    print(iter:key() .. ' ' .. iter:value()) -- value() or string() can be used

    iter:next()
end

iter:del()

leveldb.close(db_str)

-- number example
local db_num = leveldb.open(opt, 'num.db')

db_num:put('key1', 1)
db_num:put('key2', 2)
db_num:put('key3', 3.14)

iter = db_num:iterator()

iter:seekToFirst()

while(iter:valid())
do
	print(iter:key() .. ' ' .. iter:valnum()) -- get value as number
	
	iter:next()
end

iter:del()

leveldb.close(db_num)
```

License
-------
The lua-leveldb code is licensed under the terms of the MIT license. 
This means that lua-leveldb is free software and can be used for both academic and 
commercial purposes at absolutely no cost, check the LICENSE file for details.

Note that leveldb library itself (included for convenience) is licensed under:

BSD-3 (http://opensource.org/licenses/BSD-3-Clause).

Contacts
--------
- Marco Pompili
- [emarcs.org](http://www.emarcs.org)
