// Copyright 2021-22 Kirill Scherba <kirill@scherba.ru>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Teonet C library C++ definition module
//
//#pragma once
#ifndef THIS_TEONET_H
#define THIS_TEONET_H

#include "libteonet.h"
#include <cstring>
#include <string>

class Teonet;
class Teoapi;
class TeoapiCommand;
class Teoapicli;

// Main C++ Teonet reader type
typedef bool (*cpp_reader)(Teonet &teo, char *addr, void *data, int dataLen,
                           unsigned char ev);

// Teonet API command
class TeoapiCommand {
private:
  int api;
  int cmdApi;

public:
  // Constructor
  TeoapiCommand(int c_api) { api = c_api; }

  // Set command number
  TeoapiCommand *setCmd(int cmd) {
    teoApiSetCmd(cmdApi, cmd);
    return this;
  }

  // Set command name
  TeoapiCommand *setName(std::string name) {
    teoApiSetName(cmdApi, (char *)name.c_str());
    return this;
  }

  // Set command short description
  TeoapiCommand *setShort(std::string shortName) {
    teoApiSetShort(cmdApi, (char *)shortName.c_str());
    return this;
  };

  // Set command usage text
  TeoapiCommand *setUsage(std::string usage) {
    teoApiSetUsage(cmdApi, (char *)usage.c_str());
    return this;
  }

  // Set command return description
  TeoapiCommand *setReturn(std::string ret) {
    teoApiSetReturn(cmdApi, (char *)ret.c_str());
    return this;
  }

  // Set command reader
  TeoapiCommand *setReader(void *reader) {
    teoApiSetReader(cmdApi, reader);
    return this;
  }

  // Set command answer mode
  TeoapiCommand *setAnswerMode(int answerMode) {
    teoApiSetAnswerMode(cmdApi, answerMode);
    return this;
  }

  // API answer mode constants
  int answerData() { return teoApiAnswerData(); }
  int answerCmd() { return teoApiAnswerCmd(); }
  int answerPacketID() { return teoApiAnswerPacketID(); }
  int answerNo() { return teoApiAnswerNo(); }
};

// Teonet API class
class Teoapi {
private:
  int api;

public:
  // Constructor
  Teoapi(int c_teo, std::string appName, std::string appShort,
         std::string appLong, std::string appVersion) {
    char *c_appName = (char *)appName.c_str();
    char *c_appShort = (char *)appShort.c_str();
    char *c_appLong = (char *)appLong.c_str();
    char *c_appVersion = (char *)appVersion.c_str();
    api = teoApiNew(c_teo, c_appName, c_appShort, c_appLong, c_appVersion);
  }

  // get api key
  int getApi() { return api; }

  // Create new API command
  TeoapiCommand *makeAPI2() {
    int c_cmdApi = teoMakeAPI2(getApi());
    TeoapiCommand *cmdApi = new TeoapiCommand(c_cmdApi);
    return cmdApi;
  }

  // Add api command
  void add(int cmdApi) { teoApiAdd(cmdApi); }
};

// Teonet API client class
class Teoapicli {
private:
  int api;

public:
  // Constructor
  Teoapicli(int c_teo, std::string address) {
    char *c_address = (char *)address.c_str();
    api = teoApicliNew(c_teo, c_address);
  }

  // Send api command with data to teonet peer, return true if ok
  bool sendCmdTo(std::string s_cmd, void *c_data, int c_data_len,
                 void *c_reader, void *user_data = NULL) {
    return teoApicliSendCmdToStrCb(api, (char *)s_cmd.c_str(), c_data,
                                   c_data_len, c_reader, user_data);
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
  // ok. The reader will receive data from peer, it may be ommited if not
  // used
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
  Teoapicli *newAPIClient(std::string address) {
    Teoapicli *api = new Teoapicli(getTeo(), address);
    return api;
  }

  // Create Teonet API
  Teoapi *newAPI(std::string appName, std::string appShort, std::string appLong,
                 std::string appVersion) {
    Teoapi *api = new Teoapi(getTeo(), appName, appShort, appLong, appVersion);
    return api;
  }

  // Add api reader
  void addApiReader(Teoapi &api) { teoAddApiReader(getTeo(), api.getApi()); }

  // Get Teonet Event Data constant
  unsigned char evData() { return teoEvData(); }

  // The safePrintf function must be call in reader before any printf()
  // functions to safe printf() in multithreading application
  void safePrintf() { safe_printf(); }

  // Wait forever
  void waitForever() { teoWaitForever(teo); }
};

#endif /*THIS_TEONET_H */