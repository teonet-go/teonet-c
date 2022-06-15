# Teonet-c

[Teonet v5](https://github.com/teonet-go/teonet) C library and examples.

![Teonet v5 C](https://github.com/teonet-go/teonet-c/raw/main/img/teonet-c.jpg)

Teonet is designed to create client-server systems and build networks for server applications operating within a microservice architecture.

Main Teonet v5 code was created on Golang. This project make c/c++ dynamic library to use it in any C/C++ and C compatible applications.

The description of Teonet you can find at [main Teonet-go page](https://github.com/teonet-go).

The examples of this project copy Teonet-go examples functional. So look this project examples to get started with teonet-c library: [examples](https://github.com/teonet-go/teonet-c/tree/main/cmd)

## Contents

- [Make Teonet-c library and examples](make-library-and-examples)
- C language examples
  - Simple message mode
    - [Simple Clent](https://github.com/teonet-go/teonet-c/tree/main/cmd/echo/main.c)
    - [Simple Server](https://github.com/teonet-go/teonet-c/tree/main/cmd/echo-serve/main.c)
  - Command message mode
    - [Command mode Clent](https://github.com/teonet-go/teonet-c/tree/main/cmd/command/main.c)
    - [Command mode Server](https://github.com/teonet-go/teonet-c/tree/main/cmd/command-serve/main.c)
  - API message mode
    - [API Clent](https://github.com/teonet-go/teonet-c/tree/main/cmd/api/main.c)
    - [API Server](https://github.com/teonet-go/teonet-c/tree/main/cmd/api-serve/main.c)
- C++ language examples
  - Simple message mode
    - [Simple Clent](https://github.com/teonet-go/teonet-c/tree/main/cmd/cpp/echo/main.cpp)
  - Command message mode
    - [Command mode Clent](https://github.com/teonet-go/teonet-c/tree/main/cmd/cpp/command/main.cpp)
  - API message mode
    - [API Clent](https://github.com/teonet-go/teonet-c/tree/main/cmd/cpp/api/main.cpp)
    - [API Server](https://github.com/teonet-go/teonet-c/tree/main/cmd/cpp/api-serve/main.cpp)

## Make library and examples

To make Teonet-c library from sources you need install `git` `go` `gcc` and `make` applications on you computer.

Clone this repository to you project folder:

```shell
cd project_folder
git clone https://github.com/teonet-go/teonet-c.git
cd teonet-c
```

There is Makefile, use `make` command to execute it.

To build library use:

```shell
make build-library
sudo make ldconfig
```

To build examples use:

```shell
make build
```

After this you can run examples from projects root folder.  
Run the C++ API message mode client:

```shell
./cmd/cpp/api/teoapi-cpp
```

## License

[BSD](LICENSE)
