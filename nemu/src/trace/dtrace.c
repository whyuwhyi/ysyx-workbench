#include <common.h>

void init_dtrace() { Log("Device trace initialized"); }

void dtrace_read(paddr_t addr, int len, word_t data, const char *device) {
  Log("Device read from " FMT_PADDR " length %d data " FMT_WORD
      " from device %s",
      addr, len, data, device);
}

void dtrace_write(paddr_t addr, int len, word_t data, const char *device) {
  Log("Device write to " FMT_PADDR " length %d data " FMT_WORD " to device %s",
      addr, len, data, device);
}
