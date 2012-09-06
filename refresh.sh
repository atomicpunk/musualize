#!/bin/sh

rm -f *.o Makefile.in Makefile config.* configure musualizer
autogen.sh
configure
make
