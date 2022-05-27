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
                                      char *err, void *user_data);
// API reader
unsigned char runAPIReaderCb(c_api_reader c_reader, int teoApi, void *data,
                             int dataLen, char *err, void *user_data) {
  return c_reader(teoApi, data, dataLen, err, user_data);
}

// safe_printf safe printf in mulithreading appliction. Teonet-c run in
// multithread mode
void safe_printf() {
  const char *fmt = "";
  printf(fmt, 123);
}

// teoParseCmd parse input data to command and data
void *teoParseCmd(void *c_data, int c_data_len, unsigned char *c_cmd,
                  int *c_cmd_data_len) {

  // Check pointer parameters
  if (c_cmd == NULL || c_cmd_data_len == NULL) {
    return NULL;
  }
  *c_cmd_data_len = 0;
  *c_cmd = 0;

  if (c_data_len <= 0) {
    return NULL;
  }

  // Get command
  *c_cmd = ((unsigned char *)c_data)[0];
  c_data_len--;
  if (c_data_len == 0) {
    return NULL;
  }

  // Get data
  *c_cmd_data_len = c_data_len;
  return &((unsigned char *)c_data)[1];
}
