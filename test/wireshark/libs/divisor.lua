-- Create a file named by_ip/''ip_addess''.cap with all ip traffic of each ip host. (works for tshark only)
-- Dump files are created for both source and destination hosts
do
	local dir = "by_ip"
	local dumpers = {}
	
	local function init_listener()
		
		local tap = Listener.new("ip")

		-- we will be called once for every IP Header.
		-- If there's more than one IP header in a given packet we'll dump the packet once per every header
		function tap.packet(pinfo,tvb,ip)
			local ip_src, ip_dst = tostring(ip.ip_src), tostring(ip.ip_dst)
			local src_dmp, dst_dmp
			
			src_dmp = dumpers[ip_src]
			
			if not src_dmp then
				src_dmp = Dumper.new_for_current( dir .. "/" .. ip_src .. ".pcap" )
				dumpers[ip_src] = src_dmp
			end
			
			src_dmp:dump_current()
			src_dmp:flush()
			dst_dmp = dumpers[ip_dst]

			if not dst_dmp then
				dst_dmp = Dumper.new_for_current( dir .. "/" .. ip_dst .. ".pcap"  )
				dumpers[ip_dst] = dst_dmp
			end

			dst_dmp:dump_current()
			dst_dmp:flush()	
		end

		function tap.draw()
			for ip_addr,dumper in pairs(dumpers) do
				dumper:flush()
			end
		end

		function tap.reset()
			for ip_addr,dumper in pairs(dumpers) do
				dumper:close()
			end

			dumpers = {}
		end
	end

	init_listener()
end
