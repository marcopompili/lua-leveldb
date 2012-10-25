#!/bin/bash

if [ $# -lt 1 ]
then
	echo -e "USAGE: $0 <pcap-file>"
	exit 1
fi

tshark -q -X lua_script:ws_store.lua -r $1