// Copyright 2021-22 Kirill Scherba <kirill@scherba.ru>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Teonet C library C++ definition module
//
//#pragma once
#ifndef THIS_TEONET_H
#define THIS_TEONET_H

#include "../../lib/libteonet.h"
#include <cstring>
#include <string>

class Teonet;
class Teoapi;

// Main C++ Teonet reader type
typedef bool (*cpp_reader)(Teonet &teo, char *addr, void *data, int dataLen,
                           unsigned char ev);

// Teonet API client class
class Teoapi {
private:
  int api;

public:
  // Constructor
  Teoapi(int c_teo, std::string address) {
    char *c_appddress = (char *)address.c_str();
    api = teoApiClientNew(c_teo, c_appddress);
  }

  // Send api command with data to teonet peer, return true if ok
  bool sendCmdTo(std::string s_cmd, void *c_data, int c_data_len,
                 void *c_reader, void *user_data = NULL) {
    return teoApiSendCmdToStrCb(api, (char *)s_cmd.c_str(), c_data, c_data_len,
                                c_reader, user_data);
  }

  // Send api command with string to teonet peer, return true if ok
  bool sendCmdTo(std::string s_cmd, std::string msg, void *c_reader,
                 void *user_data = NULL) {
    const char *c_data = msg.c_str();
    return sendCmdTo(s_cmd, (void *)c_data, std::strlen(c_data), c_reader,
                     user_data);
  }
};

class Teonet {
private:
  int teo;
  cpp_reader reader;

  int getTeo() { return teo; }

public:
  // Constructor
  Teonet(std::string appShort) {
    char *c_appShort = (char *)appShort.c_str();
    teo = teoNew(c_appShort);
  }

  // Get Teonet-c version
  std::string version() {
    char *c_ver = teoCVersion();
    std::string ver(c_ver);
    free(c_ver);
    return ver;
  }

  // Print Teonet logo
  void logo(std::string appName, std::string appVersion) {
    teoLogo((char *)appName.c_str(), (char *)appVersion.c_str());
  }

  // Connect to teonet
  bool connect() { return teoConnect(teo); }

  // Get Teonet address
  std::string address() {
    char *c_address = teoAddress(teo);
    std::string address(c_address);
    free(c_address);
    return address;
  }

  // Connect to teonet peer with reader callback, return true if
  // ok. The reader will receive data from peer, it may be ommited if not used
  bool connectTo(std::string address, c_reader reader = NULL) {
    return teoConnectToCb(teo, (char *)address.c_str(), (void *)reader, NULL);
  }

  bool connectTo(std::string address, c_reader reader, void *c_user_data) {
    return teoConnectToCb(teo, (char *)address.c_str(), (void *)reader,
                          c_user_data);
  }

  bool connectTo(std::string address, cpp_reader reader) {
    this->reader = reader;
    return connectTo(
        address,
        [](int teo, char *addr, void *data, int dataLen, unsigned char ev,
           void *user_data) -> unsigned char {
          // Cast teonet from user_data and call saved cpp_reader
          auto teonet_ptr = static_cast<Teonet *>(user_data);
          return teonet_ptr->reader(*teonet_ptr, addr, data, dataLen, ev);
        },
        this);
  }

  // Send data to teonet peer, return true if ok
  bool sendTo(std::string address, void *data, int dataLength) {
    return teoSendTo(teo, (char *)address.c_str(), data, dataLength);
  }

  // Send string to teonet peer, return true if ok
  bool sendTo(std::string address, std::string message) {
    const char *c_msg = message.c_str();
    return sendTo(address, (void *)c_msg, std::strlen(c_msg));
  }

  // Send command with data to teonet peer, return true if ok
  bool sendCmdTo(std::string address, unsigned char cmd, void *data,
                 int dataLength) {
    return teoSendCmdTo(teo, (char *)address.c_str(), cmd, data, dataLength);
  }

  // Send command with string to teonet peer, return true if ok
  bool sendCmdTo(std::string address, unsigned char cmd, std::string message) {
    const char *c_msg = message.c_str();
    return sendCmdTo(address, cmd, (void *)c_msg, std::strlen(c_msg));
  }

  // Create Teonet API client
  Teoapi *newAPIClient(std::string address) {
    Teoapi *api = new Teoapi(getTeo(), address);
    return api;
  }

  // Get Teonet Event Data constant
  unsigned char evData() { return teoEvData(); }

  // The safePrintf function must be call in reader before any printf()
  // functions to safe printf() in multithreading application
  void safePrintf() { safe_printf(); }

  // Wait forever
  void waitForever() { teoWaitForever(teo); }
};

#endif /*THIS_TEONET_H */