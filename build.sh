#!/bin/sh

set -xe

CC="${CXX:-cc}"
CFLAGS="-Wall -ggdb -O3 -std=c11 -pedantic"

# $CC hex.c $CFLAGS -o hex -lraylib
# $CC autodiff.c $CFLAGS -o autodiff
$CC cnn.c $CFLAGS -o cnn -lm
