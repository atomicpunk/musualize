#!/bin/sh

SINK=`pactl list | egrep -A2 '^(\*\*\* )?Source #' | grep 'Name: alsa_input' | awk '{print $NF}' | head -n1`
./musualizer -r -d $SINK $@
