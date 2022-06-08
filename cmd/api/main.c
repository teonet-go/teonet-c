// Copyright 2021-22 Kirill Scherba <kirill@scherba.ru>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Teonet v5 C api command client example.
// This application connect to teonet, than connect to echo api server
// send/receive ansver every 3 seconds.
//
// build: gcc main.c `pwd`/../../lib/libteonet.so -I../../lib -o teoapi-c
//
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../lib/libteonet.h"

char *appName = "Teonet echo api client C sample application";
char *appShort = "teoapi-c";
char *apiServer = "WXJfYLDEtg6Rkm1OHm9I9ud9rR6qPlMH6NE";

// api_reader is a teonet api callback function
void api_reader(int apicli, void *data, int dataLen, char *err) {

  // The safe_printf() function must be call in reader before any printf()
  // function to safe printf() in multithreading application
  safe_printf();

  char *address = teoApiAddress(apicli);
  printf("got data: '%s', data len: %d, from: %s\n\n", (char *)data, dataLen,
         address);
  free(address);
}

int main() {

  // Application logo
  teoLogo(appName, teoCVersion());

  // Create teonet connector
  int teo = teoNew(appShort);
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

  // Connect to teonet api echo server
  ok = teoConnectTo(teo, apiServer);
  if (!ok) {
    printf("can't connect to echo server\n");
    return 1;
  }
  printf("Successfully connected to: %s\n\n", apiServer);

  // Create Teonet API client interface
  int apicli = teoApicliNew(teo, apiServer);
  if (!apicli) {
    printf("can't create api interface\n");
    return 1;
  }

  // Send messages to api echo server
  for (;;) {
    // u_char cmd = 129;
    char *cmd = "hello";
    char *msg = "Hello from teonet-c!";
    printf("send command '%s' message '%s' to %s\n", cmd, msg, apiServer);
    teoApicliSendCmdToStrCb(apicli, cmd, msg, strlen(msg), &api_reader, NULL);
    sleep(3);
  }

  // In this example we loop forever in send message. If your application
  // does not use forever loop like this than you can use the teoWaitForever
  // func teoWaitForever(teo);

  return 0;
}
