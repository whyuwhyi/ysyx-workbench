#include <common.h>

void init_mtrace() {
  Log("Memory trace initialized");
}

void mtrace_read(uint32_t addr, int len) {
  Log("Memory read from 0x%08x length %d", addr, len);
}

void mtrace_write(uint32_t addr, int len, uint32_t data) {
  Log("Memory write to 0x%08x length %d data 0x%08x", addr, len, data);
}