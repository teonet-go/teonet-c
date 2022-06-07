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

#include "../../lib/libteonet.h"

char *appName = "Teonet api server C sample application";
char *appShort = "teoapi-serve-c";
char *appLong = "This is description of Teonet api server application";

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

  // Application logo
  char *appVersion = teoCVersion();
  teoLogo(appName, teoCVersion());

  // Create teonet connector
  int teo = teoNew(appShort);
  if (!teo) {
    printf("can't initialize teonet\n");
    return 1;
  }

  // Create Teonet API
  int api = teoApiNew(teo, appName, appShort, appLong, appVersion);

  // Create API command "hello"
  int cmdApi = teoMakeAPI2(api);
  teoApiSetCmd(cmdApi, 129);                          // Command number
  teoApiSetName(cmdApi, "hello");                     // Command name
  teoApiSetShort(cmdApi, "get 'hello name' message"); // Short description
  teoApiSetUsage(cmdApi, "<name string>");            // Usage (input parameter)
  teoApiSetReturn(cmdApi, "<answer string>");      // Command return parameters
  teoApiSetReader(cmdApi, reader);                 // Command reader
  teoApiSetAnswerMode(cmdApi, teoApiAnswerData()); // Command answer mode

  teoApiAdd(cmdApi);

  // Add API reader
  teoAddApiReader(teo, api);

  // Connect to teonet
  unsigned char ok = teoConnect(teo);
  if (!ok) {
    printf("can't initialize teonet\n");
    return 1;
  }

  // Get and print your teonet address
  char *address = teoAddress(teo);
  printf("Teonet address: %s\n", address);

  // Wait forever
  teoWaitForever(teo);

  return 0;
}
