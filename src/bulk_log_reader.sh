#!/bin/bash

#
# Copyright (C) 2013 Barbora Frankova <xfrank08@stud.fit.vutbr.cz>
#
# Pcf is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Pcf is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with pcf. If not, see <http://www.gnu.org/licenses/>.
#

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
