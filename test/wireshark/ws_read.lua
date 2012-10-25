package.cpath = package.cpath .. ';../../Debug/?.so'

require 'lua-leveldb'

local wsdb = leveldb.open ('ws.db')
local iter = wsdb:iterator ()

-- initialize the iterator
iter:seekToFirst ()

--[[ iterating
while(iter:valid())
do
	iter:next()
	
	print (iter:key())
end
]]--