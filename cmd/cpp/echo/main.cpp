//
// How to build:
//   gcc main.cpp `pwd`/../../lib/libteonet.so -I../../lib -lstdc++\
//   -o teoecho-cpp
//

#include "../../../lib/libteonet.hpp"
#include <unistd.h>

const char *appName = "Teonet echo client C++ sample application";
const char *appShort = "teoecho-cpp";
const char *echoServer = "dBTgSEHoZ3XXsOqjSkOTINMARqGxHaXIDxl";

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

  return true;
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

  // Connect to teonet echo server
  ok = teo.connectTo(echoServer, reader);
  if (!ok) {
    printf("can't connect to echo server\n");
    return 1;
  }
  printf("Successfully connected to: %s\n\n", echoServer);

  // Send messages to echo server
  for (;;) {
    const char *msg = "Hello from teonet-c!";
    printf("send message '%s' to %s\n", msg, echoServer);
    teo.sendTo(echoServer, msg);
    sleep(3);
  }

  // Wait forever
  // teo.waitForever();

  return 0;
}