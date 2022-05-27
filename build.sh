#!/bin/sh

# Build lib
cd lib 
go build -buildmode c-shared -o libteonet.so .
cd ..

# Build echo client
cd cmd/echo
rm teoecho-c
gcc main.c `pwd`/../../lib/libteonet.so -I../../lib -o teoecho-c
cd ../..

# Run echo client
# cmd/echo/teoecho-c

# Build command client
cd cmd/command
rm teocommand-c
gcc main.c `pwd`/../../lib/libteonet.so -I../../lib -o teocommand-c
cd ../..

# Run echo command clien
# cmd/command/teocommand-c

# Build api client
cd cmd/api
rm teoapi-c
gcc main.c `pwd`/../../lib/libteonet.so -I../../lib -o teoapi-c
cd ../..

# Run api client
# cmd/api/teoapi-c

# Build echo server
cd cmd/echoserve
rm teoechoerve-c
gcc main.c `pwd`/../../lib/libteonet.so -I../../lib -o teoechoerve-c
cd ../..

# Run echo server
# cmd/echoserve/teoechoerve-c

# Build command server
cd cmd/commandserve
rm teocommandserve-c
gcc main.c `pwd`/../../lib/libteonet.so -I../../lib -o teocommandserve-c
cd ../..

# Run command server
cmd/commandserve/teocommandserve-c
