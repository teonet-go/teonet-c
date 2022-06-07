// Copyright 2021-22 Kirill Scherba <kirill@scherba.ru>. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Teonet C library callabacks definition module
//
//#pragma once
#ifndef THIS_C_CALLBACKS_H
#define THIS_C_CALLBACKS_H

// Main reader type
typedef unsigned char (*c_reader)(int teo, char *addr, void *data, int dataLen,
                                  unsigned char ev, void *user_data);
// API client reader type
typedef unsigned char (*c_api_reader)(int teoApi, void *data, int dataLen,
                                      char *err, void *user_data);

// API server reader type
// typedef unsigned char (*c_api_s_reader)(int teoApi, void *c, void *p,
//                                         void *data, int dataLen, char *err,
//                                         void *user_data);
typedef void *(*c_api_s_reader)(int cmdApi, char *addr, void *data, int dataLen,
                                void *userdat, int *outDataLen);

// Main reader
unsigned char runReaderCb(c_reader c_reader, int teo, char *addr, void *data,
                          int dataLen, unsigned char ev, void *user_data);

// API client reader
unsigned char runAPIReaderCb(c_api_reader c_reader, int teoApi, void *data,
                             int dataLen, char *err, void *user_data);

// API server reader
void *runAPIsReaderCb(c_api_s_reader c_reader, int teoApiCmd, char *addr,
                      void *data, int dataLen, void *user_data,
                      int *outDataLen);

#ifdef __cplusplus
extern "C" {
#endif

// safe_printf safe printf in mulithreading appliction. Teonet-c run in
// multithread mode
extern void safe_printf();

// teoParseCmd parse input data to command and data
extern void *teoParseCmd(void *c_data, int c_data_len, unsigned char *c_cmd,
                         int *c_cmd_data_len);

#ifdef __cplusplus
}
#endif

#endif /*THIS_C_CALLBACKS_H */
