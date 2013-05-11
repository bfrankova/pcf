#!/bin/bash

function process {
	for FILE in $(ls $1)
	do
		SOURCE_LOG="$1$FILE"
		if [ -s $SOURCE_LOG ] && [ -f $SOURCE_LOG ] && [ "$FILE" != "log_reader.log" ]; then
			TARGET=${FILE%".log"}$2
			printf "processing "$TARGET" ... "
			./log_reader  $SOURCE_LOG > /dev/null
			if [ -s www/graph/log_reader.svg ]; then
				echo -e "\e[1;32mOK\e[0m"
				mv graph/log_reader.gp bulk_output/gp/$TARGET.gp
				mv www/graph/log_reader.svg bulk_output/svg/$TARGET.svg
			else
				echo -e "\e[1;31mno output, skipping\e[0m"
			fi
		fi
	done
}

process "log/" "-tcp"
process "log/javascript/" "-js"
process "log/icmp/" "-icmp"
