#!/bin/sh

SINK=`pactl list | egrep -A2 '^(\*\*\* )?Source #' | grep 'Name: .*\.monitor$' | awk '{print $NF}' | head -n1`
./musualizer -v -d $SINK --rate=192000 -w hamming -t "8:0:-1" $@
