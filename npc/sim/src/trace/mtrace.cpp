#include <common.h>

void init_mtrace() { Log("Memory trace initialized"); }

void mtrace_read(paddr_t addr, int len) {
  Log("Memory read from " FMT_PADDR " length %d", addr, len);
}

void mtrace_write(paddr_t addr, int len, word_t data) {
  Log("Invalid memory write to " FMT_PADDR " length %d data 0x%08x", addr, len,
      data);
}
