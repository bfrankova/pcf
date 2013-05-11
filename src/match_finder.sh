#!/bin/bash

printf "\t"
for FILE2 in $(ls bulk_output/gp)
	do
		printf "$FILE2\t"
	done
	echo

for FILE1 in $(ls bulk_output/gp)
do
	printf "$FILE1\t"
	for FILE2 in $(ls bulk_output/gp)
	do
		RESULT=`./coupler.py bulk_output/gp/$FILE1 bulk_output/gp/$FILE2 | awk '/Average/ { print $4; }'`
		printf "$RESULT\t"
	done
	echo
done
