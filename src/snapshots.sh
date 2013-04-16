#!/usr/bin/env bash

rm -rf graph/* log/* www/graph/*

mkdir graph/icmp log/icmp www/graph/icmp

while [ 1 ]
do
	TIME=`date "+%Y-%m-%d-%H-%M"`
	tar cvf "graphs-$TIME.tar" graph/ log/ www/graph/
	echo "$TIME"
	sleep 1m
done
