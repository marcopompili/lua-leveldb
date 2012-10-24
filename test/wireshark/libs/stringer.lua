-- Convert http packet to raw HTTP text log
function print_raw_http_packet(ascii_header, ascii_body)
	print(dexer(tostring(ascii_header.value)))
	
	if ascii_body
	then
		print(dexer(tostring(ascii_body.value)))
	end
end

-- Convert http packet to a JSON string
function print_json_http_packet(ascii_header, ascii_body)
	local text_header = dexer(tostring(ascii_header.value))
	
	local json = '{'

	for label, value in string.gmatch(text_header, "([%w-_]*):%s([%w%p\ *]*)") do
		json = json .. '"' .. label .. '":"' .. value .. '"'
		
		if string.gmatch(value, "^\n$")
		then
			json = json .. ',\n'
		end
	end
	
	if ascii_body
	then
		local text_body = dexer(tostring(ascii_body.value))
		
		json = json .. '"Body":"' .. text_body .. '"\n },'
	else
		json = json .. '},\n'
	end
	
	print(json)
end