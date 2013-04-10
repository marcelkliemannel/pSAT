#!/bin/bash

if [ $# == 3 ]; then
	if [ "$2" == "1" ]; then
		ALGORITHM="rots"
	else
		if [ "$2" == "2" ]; then
			ALGORITHM="ilssa"
		else
			echo "The algorithm parameter must be 1 (RoTS) or 2 (ILS/SA)!"
			exit 1
		fi
	fi
	
	if [ -f "psat" ];
	then
		./psat -f $1 -a $ALGORITHM -r $3
	else
		echo "pSAT binary not found. Compile first?"
	fi
else
	echo "There must be 3 parameters: \"instance file path\" \"algorithm (1 [RoTS] or 2 [ILS/SA]).\" \"random seed\""
fi
