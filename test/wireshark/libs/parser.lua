-- Gets a cookie value
function get_cookie_field(cookie, field)
	for label, value in string.gmatch(cookie, "([%w-_]*)=([%w%p]*)") do
		if field == label
		then
			--print(label .. ' = ' .. value .. "\n") -- debug mode
			return value
		end
	end
end

-- Converts an ascii hex header into a lua table filled with clear text
function header_to_table(ascii_header)
	local text_header = dexer(tostring(ascii_header.value))
	local table = {}
	
	local call = string.match(text_header, "([GET|POST]*%s+[%w%p]*)")

	--print (call)

	table['Call'] = call

	for label, value in string.gmatch(text_header, "([%w-_]*):%s([%w%p\ *]*)") do
		--print(label .. ' ' .. value) --debug mode
		table[label] = value
	end
	
	return table
end