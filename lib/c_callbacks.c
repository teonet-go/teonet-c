#include <stdio.h>

// Main reader type
typedef unsigned char (*c_reader)(int teo, char *addr, void *data, int dataLen,
                                  unsigned char ev);
// Main reader
unsigned char runReaderCb(c_reader c_reader, int teo, char *addr, void *data,
                          int dataLen, unsigned char ev) {
  return c_reader(teo, addr, data, dataLen, ev);
}

// API reader type
typedef unsigned char (*c_api_reader)(int teoApi, void *data, int dataLen,
                                      char *err);
// API reader
unsigned char runAPIReaderCb(c_api_reader c_reader, int teoApi, void *data,
                             int dataLen, char *err) {
  return c_reader(teoApi, data, dataLen, err);
}

// safe_printf format string
const char *fmt = "";

// safe_printf safe printf in mulithreading appliction. Teonet-c run in
// multithread mode
void safe_printf() { printf(fmt, 123); }
