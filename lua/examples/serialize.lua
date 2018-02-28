leveldb = require 'lualeveldb'

--[[
Lua >=5.3.3 extended the format option "%q" to work also with
numbers (plus nil and Booleans), again writing them in a
proper way to be read back by Lua.
]]--
function serialize(o)
   local t = type(o)
   if t == "number"
      or t == "string"
      or t == "boolean"
      or t == "nil"
   then
      return string.format("%q", o)
   else
      error("cannot serialize a: " .. type(o))
   end
end

function deserialize(s)
   local n = tonumber(s)
   if n ~= nil then
      return n
   elseif s == 'true' then
      return true
   elseif s == 'false' then
      return false
   else
      return s
   end
end

local opt = leveldb.options()
opt.createIfMissing = true
opt.errorIfExists = false

local db = leveldb.open(opt, 'types.db')

-- serialization example: simple serialization can be used for storing values, tables too.

assert(db:put("string", "Oh hai Mark!"))
assert(db:put("number", serialize(123456)))
assert(db:put("boolean", serialize(true)))

local function check(val, expected)
   assert(type(deserialize(val)) == expected, "expected " .. expected .. ", found: " .. type(val))
end

check(db:get("string"), "string")
check(db:get("number"), "number")
check(db:get("boolean"), "boolean")

print("all type tests are fine!")

leveldb.close(db)
