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
