// Copyright 2021-22 Kirill Scherba <kirill@scherba.ru>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Teonet v5 C command server example.
// This application connect to teonet, than wait echo clients send send message
// and ansver it.
//
// build: gcc main.c `pwd`/../../lib/libteonet.so -I../../lib -o teoechoerve-c
//
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../lib/libteonet.h"

char *appName = "Teonet command server C sample application";
char *appShort = "teocommand-serve-c";

// reader is a teonet channels callback function
unsigned char reader(int teo, char *addr, void *data, int dataLen,
                     unsigned char ev) {

  // Check teonet event
  if (ev != teoEvData()) {
    return 0;
  }

  // The safe_printf() function must be call in reader before any printf()
  // function to safe printf() in multithreading application
  safe_printf();

  // Parse command data
  int cmdDataLen;
  unsigned char cmd;
  void *cmdData = teoParseCmd(data, dataLen, &cmd, &cmdDataLen);

  // Process commands
  switch (cmd) {
  case 129:
    printf("got data: '%s', data len: %d, from: %s\n", (char *)cmdData,
           cmdDataLen, addr);
    teoSendTo(teo, addr, cmdData, cmdDataLen);
    break;

  default:
    printf("got wrong command\n");
  }

  return 1;
}

int main() {

  // Application logo
  teoLogo(appName, teoCVersion());

  // Create teonet connector
  int teo = teoNewCb(appShort, reader);
  if (!teo) {
    printf("can't initialize teonet\n");
    return 1;
  }

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
