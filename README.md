lua-leveldb
===========
Lua bindings for Google's LevelDB key/store database.

[![Build Status](https://travis-ci.org/marcopompili/lua-leveldb.svg?branch=master)][travis]

From the GoogleCode page:
"The LevelDB library provides a persistent key value store. Keys and values are arbitrary byte arrays.
The keys are ordered within the key value store according to a user-specified comparator function."

I wrote this extension because I needed a fast embeddable key/value database for tcp/ip packet analysis. I had to analyze big files in pcap format and divide the traffic using some specific rules. This simple Lua extension allows you to access LevelDB from Lua in a quite simple way. Lua is also used as extension language for WireShark.

Most of the basic options and functions are supported right now, but still not the full set of operations permitted by LevelDB. Take a look at the examples below.

Installation
------------
The library is packed like a luarock extension, check out [luarocks.org](http://luarocks.org/).
  * Before installing be sure that you got LevelDB correctly installed for your Linux distribution.
  * To install execute make.sh as root, this script should build the library and install it as a luarock package.
  * To remove the library use remove.sh (as root) to remove the package and delete the built files.
  * You can rebuild lua-leveldb using the rebuild.sh script.

You can install lua-leveldb with the nice luarocks packaging system (if you got Lua >= 5.2):
```
sudo luarocks install lua-leveldb
```

Or if you want to build the latest version locally use the make script like this:
```
sudo ./make.sh 0.3 1
```

Or if you want to build the latest version locally use the make like this (for Lua = 5.1):
```
make
```

Manual install (on Debian 8):
```
sudo cp liblua-leveldb.so /usr/local/lib/
sudo mkdir /usr/local/lib/lua/5.1/
cd /usr/local/lib/lua/5.1/
sudo ln -s ../../liblua-leveldb.so lua-leveldb.so
```

Note - Library name collision with leveldb.so
---------------------------------------------

When LevelDB libraries are installed, there is a file with name `libleveldb.so` in `/usr/lib/`. When doing a `require 'leveldb'` in your lua-code, the loader code will find that file first. To make the loading unambiguous, it is necessary to refer this library as `liblua-leveldb.so`.
See the below example code for `require 'lua-leveldb'`.

Support
-------
The extension still not support the full set of operations permitted by the LevelDB library.

These are the current possible operation:
  * Basic open/close/repair database operations supported.  
  * Basic database write, read and delete operations.
  * Partial access to most important options settings is supported.
  * Iterators with seek and iteration operations are supported.
  * Atomic batch updates supported.
  * Experimental support for unique values.
  * Added support for storing string, numeric and boolean values.
  * ToString support for various objects.

Basic Example
-------------
This is a simple example about how to use the Lua extension for Google's LevelDB, just putting some data and getting it back:

```lua
local leveldb = require 'lua-leveldb'

opt = leveldb.options()
opt.createIfMissing = true
opt.errorIfExists = false

local test_key = 'key1'
local test_val = 'value1'

print ('opening test.db')
testdb = leveldb.open(opt, 'test.db')

if leveldb.check(testdb)
then
    if testdb:put(test_key, test_val)
    then
        print ('key1: '.. testdb:get(test_key))
    end
end

leveldb.close(testdb)

testdb = leveldb.open(opt, 'test.db')
testdb:put('key2', 123456)

print ('key2: ' .. testdb:get('key2'))

leveldb.close(testdb)
```

Iterator Example
----------------
An example using iterators:

```lua
local leveldb = require 'leveldb'

local opt = leveldb.options()
opt.createIfMissing = true
opt.errorIfExists = false

-- db example
local db = leveldb.open(opt, 'iterator.db')

assert(db:put('key1s', 'value1'))
assert(db:put('key1n', 1))
assert(db:put('key2s', 'value2'))
assert(db:put('key2n', 2))
assert(db:put('key3s', 'value3'))
assert(db:put('key3n', 3.14))
assert(db:put('keyb - true', true))
assert(db:put('keyb - false', false))

local iter = db:iterator()

iter:seekToFirst()

while(iter:valid())
do
    local value = iter:value();
    print(iter:key() .. ' => ' .. '(' .. type(value) .. ') ' .. tostring(value))

    iter:next()
end

iter:del()

leveldb.close(db)
```

Types
-----
At the moment only string, numbers and booleans can be stored.
Here an examples of dealing with types.
```lua
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
```

Unit tests
-----------
The examples above have been adapted to be used as unit tests.

Notes
-----
Lua-leveldb is compatible with Lua 5.1 and newer.

License
-------
The lua-leveldb code is licensed under the terms of the MIT license. 
This means that lua-leveldb is free software and can be used for both academic and 
commercial purposes at absolutely no cost, check the LICENSE file for details.

Note that LevelDB library itself (included for convenience) is licensed under [BSD-3](http://opensource.org/licenses/BSD-3-Clause).

Donate
------
 - [![PayPal](https://img.shields.io/badge/%24-paypal-f39c12.svg)][paypal-donations] 
 - BTC: 1DVAWeg259xjFWToQ3bycyE19Qy7dAVDKR

Contacts
--------
- [twitter](https://twitter.com/pompilimrc)
- [stackoverflow](https://stackoverflow.com/users/2555321/marcs)

[travis]: https://travis-ci.org/marcopompili/lua-leveldb
[paypal-donations]: https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=ZCEWXHNPK8JXN
