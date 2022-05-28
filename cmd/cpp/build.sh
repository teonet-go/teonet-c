#!/bin/sh

# Build echo client
rm teoecho-cpp
gcc main.cpp `pwd`/../../lib/libteonet.so -I../../lib -lstdc++ -o teoecho-cpp

# Run echo client
./teoecho-cpp
