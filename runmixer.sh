#!/bin/sh

SINK=`pactl list | egrep -A2 '^(\*\*\* )?Source #' | grep 'Name: .*\.monitor$' | awk '{print $NF}' | head -n1`
./musualizer -d $SINK $@
