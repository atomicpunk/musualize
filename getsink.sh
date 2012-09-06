#!/bin/sh

pactl list | egrep -A2 '^(\*\*\* )?Source #' | grep 'Name: .*\.monitor$' | awk '{print $NF}' | tail -n1
