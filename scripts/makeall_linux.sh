#!/bin/bash

DIR=../linux_*

for f in $DIR
do
	echo -e "\033[32m[$f]\033[0m"
	if [ -d $f ]; then
		if [[ $f != *"lib"* ]]; then
			if [[ $f = *"rpi"* ]]; then
				if which /opt/vc/bin/vcgencmd ; then		
					cd "$f"
					if [ -f Makefile ]
						then
							make $1 $2 || exit
					fi
					cd -
				else
					echo -e "\033[33mSkipping...\033[0m"
					continue
				fi
			fi
			cd "$f"
			if [ -f Makefile ]
				then
					make $1 $2 || exit
			fi
			cd -
		fi
	fi
done
        
