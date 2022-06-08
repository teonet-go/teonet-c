// Copyright 2021-22 Kirill Scherba <kirill@scherba.ru>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Teonet v5 C echo command client example.
// This application connect to teonet, than connect to echo command server
// send/receive ansver every 3 seconds.
//
// build: gcc main.c `pwd`/../../lib/libteonet.so -I../../lib -o teoapi-c
//
#include "../../../lib/libteonet.hpp"
#include <unistd.h>

const char *appName = "Teonet echo api client C++ sample application";
const char *appShort = "teoapi-cpp";
const char *echoApiServer = "WXJfYLDEtg6Rkm1OHm9I9ud9rR6qPlMH6NE";

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

  // Create teonet connector
  Teonet teo(appShort);
  teo.logo(appName, teo.version());

  // Connect to teonet
  bool ok = teo.connect();
  if (!ok) {
    printf("can't initialize teonet\n");
    return 1;
  }

  // Get and print your teonet address
  printf("Teonet address: %s\n", teo.address().c_str());

  // Connect to teonet api echo server
  ok = teo.connectTo(echoApiServer);
  if (!ok) {
    printf("can't connect to echo server\n");
    return 1;
  }
  printf("Successfully connected to: %s\n\n", echoApiServer);

  // Create Teonet API client interface
  Teoapicli *apicli = teo.newAPIClient(echoApiServer);
  if (!apicli) {
    printf("can't create Api Client\n");
    return 1;
  }

  // Send messages to echo server
  for (;;) {
    // u_char cmd = 129;
    const char *cmd = "hello";
    const char *msg = "Hello from teonet-c++!";
    printf("send command '%s' message '%s' to %s\n", cmd, msg, echoApiServer);
    apicli->sendCmdTo(cmd, msg, (void *)&api_reader);
    sleep(3);
  }

  // In this example we loop forever in send message. If your application
  // does not use forever loop like this than you can use the teoWaitForever
  // func teoWaitForever(teo);

  return 0;
}
