// Copyright 2021-22 Kirill Scherba <kirill@scherba.ru>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Teonet v5 C api server example.
// This application connect to teonet, than wait echo clients send send message
// and ansver it.
//
// build: gcc main.c `pwd`/../../lib/libteonet.so -I../../lib -o teoechoerve-c
//
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../../lib/libteonet.hpp"

const char *appName = "Teonet api server C++ sample application";
const char *appShort = "teoapi-serve-cpp";
const char *appLong =
    "This is description of Teonet api c++ server application";

// API Command reader
void *reader(int cmdApi, char *addr, void *data, int dataLen, void *userdat,
             int *outDataLen) {

  // The safe_printf() function must be call in reader before any printf()
  // function to safe printf() in multithreading application
  safe_printf();

  printf("got from %s, \"%s\", len: %d\n\n", addr, (char *)data, dataLen);

  // Create output data and set output data len. Output data shoud be allocated
  // by malloc function and will be free by Teonet-C Library after return
  void *outData = NULL;
  if (dataLen > 0) {
    outData = malloc(dataLen);
    memcpy(outData, data, dataLen);
  }
  if (outDataLen) {
    *outDataLen = dataLen;
  }

  return outData;
}

int main() {

  // Create teonet connector
  Teonet teo(appShort);
  std::string appVersion = teo.version();
  teo.logo(appName, appVersion);

  // Create Teonet API
  Teoapi teoapi(teo.getTeo(), appName, appShort, appLong, appVersion);

  // Create API command "hello"
  TeoapiCommand *cmdApi = teoapi.makeAPI2()
                              ->setCmd(129)
                              ->setName("hello")
                              ->setShort("get 'hello name' message")
                              ->setUsage("<name string>")
                              ->setReturn("<answer string>")
                              ->setReader((void *)reader)
                              ->setAnswerMode(cmdApi->answerData());

  teoapi.add(cmdApi);

  // Add API reader
  teo.addApiReader(teoapi);

  // Connect to teonet
  bool ok = teo.connect();
  if (!ok) {
    printf("can't initialize teonet\n");
    return 1;
  }

  // Get and print your teonet address
  printf("Teonet address: %s\n", teo.address().c_str());

  // Wait forever
  teo.waitForever();

  return 0;
}
