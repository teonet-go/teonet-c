#!/bin/sh
cd lib 
go build -buildmode c-shared -o libteonet.so .
cd ../cmd/echo
gcc main.c ../../lib/libteonet.so -I../../lib -o teoecho-c 