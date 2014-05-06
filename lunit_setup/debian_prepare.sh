#!/bin/bash
echo -e "Preparing lunit testing environment..."

LUAROCKS="/usr/bin/luarocks"

if [ -f $LUAROCKS ]
then
	echo -e "Found luarocks"
else
	echo -e "Installing luarocks"
	sudo aptitude install luarocks
fi

$LUAROCKS list

if [ ! -f /usr/local/bin/lunit.sh ]
then
	echo -e "lunitx not found!\nInstalling..."
	sudo luarocks install lunitx
else
	echo -e "lunitx already installed, nothing else to do."
fi
