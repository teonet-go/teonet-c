#!/bin/sh

# Build lib
cd lib 
go build -buildmode c-shared -o libteonet.so .
cd ..

# Build echo
cd cmd/echo
gcc main.c `pwd`/../../lib/libteonet.so -I../../lib -o teoecho-c
cd ../..

# Run echo
# cmd/echo/teoecho-c

# Build command
cd cmd/command
gcc main.c `pwd`/../../lib/libteonet.so -I../../lib -o teocommand-c
cd ../..

# Run echo command
# cmd/command/teocommand-c
