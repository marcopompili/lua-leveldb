package.cpath = package.cpath .. ';../../Release/?.so'
package.path = package.path .. ';./libs/?.lua'

require 'lua-leveldb'
require 'dhexter'
require 'parser'
require 'stringer'

do
	-- Timestamp
	local frame_time = Field.new('frame.time')
	local frame_time_epoch = Field.new('frame.time_epoch')
	
	-- TCP
	local tcp_port_src = Field.new('tcp.srcport')
	local tcp_port_dst = Field.new('tcp.dstport')
	
	-- http content type
	local http_content_type = Field.new('http.content_type')

	-- http header
	local http_extractor_f = Field.new('http')

	-- http text body
	local http_data_text_f = Field.new('data-text-lines')

	-- http media body
	local http_media_f = Field.new('media')

	local function init_http_listeners()
		local http_req_tap = Listener.new('http', 'http.request')
		--local http_res_tap = Listener.new("http", "http.response")
		
		-- WS open database
		wsdb = leveldb.open('ws.db')

		function http_req_tap.reset()
			-- WS close database
			leveldb.close(wsdb)
		end
		
		function http_req_tap.packet(pinfo, tvb)
			--print_raw_http_packet(http_extractor_f(), http_data_text_f())
			--print_json_http_packet(http_extractor_f(), http_data_text_f())
			--store_http_packet(tcp_port_src(), tcp_port_dst(), http_extractor_f(), http_data_text_f())
			
			local frame_epoch = tostring(frame_time_epoch())
 			local http_header = dexer(tostring(http_extractor_f().value))
 			if http_data_text_f() ~= nil
 			then
 				local http_body = dexer(tostring(http_data_text_f().value))
 				wsdb:put(frame_epoch, http_header .. http_body)
 			else
 				wsdb:put(frame_epoch, http_header)
 			end
 			
 			
		end

		--[[function http_res_tap.packet(pinfo, tvb)
			if http_content_type()
			then
				if string.find(tostring(http_content_type()), "text/html") ~= nil
				then
					--Convertire in scrittura di file html
					--print(fromhex(tostring(http_data_text_f().value)))
				end
			end
		end]]--

        end
	init_http_listeners()
end