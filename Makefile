PROJECTNAME=$(shell basename "$(PWD)")
LIB=$(PWD)/lib
INC=$(PWD)/lib

.DEFAULT_GOAL := all

hello:
	@echo "Teonet-C builder"

## build-lib: Build teonet-c library
build-lib:
	#
	# Build library
	cd lib && go build -buildmode c-shared -o $(LIB)/libteonet.so .
	#
	# Execute next command once, after first library build: 
	# sudo make ldconfig
	#

## ldconfig: Execute this command once, after first build library
ldconfig:
	ldconfig `pwd`/lib

## build: Build teonet-c library and examples
build: build-lib
	#
	# C examples
	# Build C echo client
	cd cmd/echo && gcc main.c -L$(LIB) -lteonet -I$(INC) -o teoecho-c

	#
	# Build C command client
	cd cmd/command && gcc main.c -L$(LIB) -lteonet -I$(INC) -o teocommand-c

	#
	# Build C API client
	cd cmd/api && gcc main.c -L$(LIB) -lteonet -I$(INC) -o teoapi-c

	#
	# Build C echo server
	cd cmd/echo-serve && gcc main.c -L$(LIB) -lteonet -I$(INC) -o teoecho-serve-c

	#
	# Build C command server
	cd cmd/command-serve && gcc main.c -L$(LIB) -lteonet -I$(INC) -o teocommand-serve-c

	#
	# Build C api server
	cd cmd/api-serve && gcc main.c -L$(LIB) -lteonet -I$(INC) -o teoapi-serve-c

	#
	# C++ examples
	# Build C++ echo client
	cd cmd/cpp/echo && gcc main.cpp -L$(LIB) -lteonet -I$(INC) -lstdc++ -o teoecho-cpp

	#
	# Build C++ command client
	cd cmd/cpp/command && gcc main.cpp -L$(LIB) -lteonet -I$(INC) -lstdc++ -o teocommand-cpp

	#
	# Build C++ api client
	cd cmd/cpp/api && gcc main.cpp -L$(LIB) -lteonet -I$(INC) -lstdc++ -o teoapi-cpp

	#

## clean: Clean remove all file maked by build commands
clean:
	cd lib && rm libteonet.so libteonet.h
	cd cmd/echo && rm teoecho-c
	cd cmd/command && rm teocommand-c
	cd cmd/api && rm teoapi-c
	cd cmd/echo-serve && rm teoecho-serve-c
	cd cmd/command-serve && rm teocommand-serve-c
	cd cmd/cpp/echo && rm teoecho-cpp
	cd cmd/cpp/command && rm teocommand-cpp
	go clean

run:
	# go run main.go
	@echo $1

compile:
	# echo "Compiling for every OS and Platform"
	# GOOS=linux GOARCH=arm go build -o bin/main-linux-arm main.go
	# GOOS=linux GOARCH=arm64 go build -o bin/main-linux-arm64 main.go
	# GOOS=freebsd GOARCH=386 go build -o bin/main-freebsd-386 main.go

## complete: Set auto complete for make commade
complete:
	# complete -W 'build-lib build ldconfig clean' make

list:
	@grep '^[^#[:space:]].*:' Makefile

help: Makefile
	@echo "Choose a command run in "$(PROJECTNAME)":"
	@sed -n 's/^##//p' $< | column -t -s ':' |  sed -e 's/^/ /'	

all: hello help