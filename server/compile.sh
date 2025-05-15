#!/bin/bash

set -e

LIBSDIR="libs"
OUTDIR="build"

mkdir -p $OUTDIR

# Kompilacja core
gcc -c $LIBSDIR/core.c -o $OUTDIR/core.o
ar rcs $OUTDIR/libcore.a $OUTDIR/core.o

# Kompilacja dllist
gcc -c $LIBSDIR/dllist.c -o $OUTDIR/dllist.o
ar rcs $OUTDIR/libdllist.a $OUTDIR/dllist.o

# Kompilacja głównego programu
gcc socket-server.c -I$LIBSDIR \
    -L$OUTDIR -lcore -ldllist \
    -pthread -o $OUTDIR/server