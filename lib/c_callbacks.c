#include <stdio.h>

typedef unsigned char (*c_reader)(int teo, char *addr, void *data, int dataLen,
                                  unsigned char ev);

unsigned char runReaderCb(c_reader c_reader, int teo, char *addr, void *data,
                          int dataLen, unsigned char ev) {
  return c_reader(teo, addr, data, dataLen, ev);
}

// safe_printf safe printf in mulithreading appliction. Teonet-c run in
// multithread mode
char *fmt = "";
void safe_printf() { printf(fmt, 123); }
