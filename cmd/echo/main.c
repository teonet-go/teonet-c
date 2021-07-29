// Copyright 2021 Kirill Scherba <kirill@scherba.ru>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Teonet v4 C echo client example. 
// This application connect to teonet, than connect to echo server send/receive 
// ansver every 5 seconds.
//
// build: gcc main.c ../../lib/libteonet.so -I../../lib -o teoecho-c
//
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <libteonet.h>

char *appName    = "Teonet echo client/server C sample application";
char *appShort   = "teoecho-c";
char *appVersion = "0.2.1";

char *echoServer = "dBTgSEHoZ3XXsOqjSkOTINMARqGxHaXIDxl";

// reader is a teonet channels callback function 
unsigned char reader(int teo, char *addr, void* data, int dataLen, unsigned char ev) {
    
    // Check teonet event
    if(ev != teoEvData()) {
        return 0;
    }

    // The safe_printf() function must be call in reader before any printf() 
    // function to safe printf() in multithreading application
    safe_printf();

    printf("got data: '%s', data len: %d, from: %s\n\n", data, dataLen, addr);
    return 1;
}

int main() {

    // Application logo
	teoLogo(appName, appVersion);

    // Create teonet connector
    int teo = teoNew(appShort);
    if (!teo) {
        printf("can't initialize teonet\n");
        return 1;
    }

    // Connect to teonet
    unsigned char ok = teoConnect(teo);
    if (!teo) {
        printf("can't initialize teonet\n");
        return 1;
    }

    // Get and print your teonet address
    char *address = teoAddress(teo);
    printf("Teonet address: %s\n", address);

    // Connect to teonet echo server
    // ok = teoConnectTo(teo, echoServer);
    ok = teoConnectToCb(teo, echoServer, &reader);
    if (!teo) {
        printf("can't connect to echo server\n");
        return 1;
    }
    printf("Successfully connected to: %s\n\n", echoServer);

    // Send messages to echo server
    for(;;) {
        char *msg = "Hello from teonet-c!";
        printf("send message '%s' to %s\n", msg, echoServer);
        teoSendTo(teo, echoServer, msg, strlen(msg));
        sleep(5);
    }

    // In this example we loop forever in send message for. If your application
    // does not use for like this than you can use the teoWaitForever func
    // teoWaitForever(teo);

    return 0;
}

