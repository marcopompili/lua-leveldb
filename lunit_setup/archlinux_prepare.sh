#!/bin/bash

echo -e "\e[1;37mPreparing lunit testing environment for Archlinux...\e[0m"

LUAROCKS="/usr/bin/luarocks-5.1"

if [ -f $LUAROCKS ]
then
	echo -e "\e[1;32mLuarocks 5.1 found!\e[0m"
else
	echo -e "\e[1;33mInstalling luarocks 5.1\e[0m"
	sudo pacman -S luarocks5.1
fi

LUNITX="/usr/bin/lunit.sh"

if [ -f $LUNITX ]
then
	echo -e "\e[1;32mlunitx is already installed, nothing to do!\e[0m"
else
	echo -e "\e[1;33mInstalling lunitx extension for lua 5.1\e[0m"
	sudo $LUAROCKS install lunitx
	
	if [ -f $LUNITX ]
	then
		echo -e "\e[1;32mInstallation complete!\e[0m"
	else
		echo -e "\e[1;31mInstallation error!\e[0m"
	fi
fi
