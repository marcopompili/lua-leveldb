-- LUA HEX DECODER
function dexer(str)
   	return (str:gsub('..', function (cc)
       	return string.char(tonumber(cc, 16))
   	end))
end

-- LUA HEX ENCODER
function hexer(str)
  	return (str:gsub('.', function (c)
       	return string.format('%02X', string.byte(c))
   	end))
end