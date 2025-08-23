#include <common.h>

void init_mtrace() { Log("Memory trace initialized"); }

void mtrace_read(paddr_t addr, int len, word_t data) {
  Log("Memory read from " FMT_PADDR ": length %d, data " FMT_WORD ".", addr,
      len, data);
}

void mtrace_write(paddr_t addr, int len, word_t data) {
  Log("Memory write to " FMT_PADDR ": length %d ,data " FMT_WORD ".", addr, len,
      data);
}
