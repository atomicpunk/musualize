#!/bin/sh

SINK=`pactl list | egrep -A2 '^(\*\*\* )?Source #' | grep 'Name: .*\.monitor$' | awk '{print $NF}' | tail -n1`
/home/tebrandt/workspace/musualize/musualizer -r -d $SINK $@
