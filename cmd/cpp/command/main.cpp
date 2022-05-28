// Copyright 2021-22 Kirill Scherba <kirill@scherba.ru>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Teonet v5 C echo command client example.
// This application connect to teonet, than connect to echo command server
// send/receive ansver every 3 seconds.
//
// build: gcc main.c `pwd`/../../lib/libteonet.so -I../../lib -o teocommand-c
//
#include "../teonet.hpp"
#include <unistd.h>

const char *appName = "Teonet echo command client C++ sample application";
const char *appShort = "teocommand-cpp";
const char *echoComServer = "WXJfYLDEtg6Rkm1OHm9I9ud9rR6qPlMH6NE";

// reader is a teonet channels callback function
bool reader(Teonet &teo, char *addr, void *data, int dataLen,
            unsigned char ev) {

  // Check teonet event
  if (ev != teo.evData()) {
    return 0;
  }

  // The safe_printf() function must be call in reader before any printf()
  // function to safe printf() in multithreading application
  teo.safePrintf();

  printf("got data: '%s', data len: %d, from: %s\n\n", (char *)data, dataLen,
         addr);

  return 1;
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

  // Connect to teonet command echo server
  ok = teo.connectTo(echoComServer, reader);
  if (!ok) {
    printf("can't connect to echo server\n");
    return 1;
  }
  printf("Successfully connected to: %s\n\n", echoComServer);

  // Send messages to echo server
  for (;;) {
    u_char cmd = 129;
    const char *msg = "Hello from teonet-c!";
    printf("send command %d message '%s' to %s\n", cmd, msg, echoComServer);
    teo.sendCmdTo(echoComServer, 129, msg);
    sleep(3);
  }

  // In this example we loop forever in send message. If your application
  // does not use forever loop like this than you can use the teoWaitForever
  // func teoWaitForever(teo);

  return 0;
}
