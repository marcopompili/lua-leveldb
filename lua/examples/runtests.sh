#!/bin/bash

if [ "$#" -eq 1 ]
then
    INTR=$1
else
    INTR=lua
fi

for f in *.lua; do 
	echo -e "\nProcessing $f file..";
	$INTR $f
done

echo "examples ran with:"
$INTR -v
