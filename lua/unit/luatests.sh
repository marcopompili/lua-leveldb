#!/bin/bash

for f in *.lua; do 
	echo -e "\nProcessing $f file..";
	lua $f
done
