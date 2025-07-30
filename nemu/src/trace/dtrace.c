#include <common.h>
#include <memory/paddr.h>

void init_dtrace() { Log("Device trace initialized"); }

void dtrace_read(paddr_t addr, int len, const char *device) {
  Log("Device read from " FMT_PADDR " length %d from device %s", addr, len,
      device);
}

void dtrace_write(paddr_t addr, int len, word_t data, const char *device) {
  Log("Device write to " FMT_PADDR " length %d data 0x%08x to device %s", addr,
      len, data, device);
}
