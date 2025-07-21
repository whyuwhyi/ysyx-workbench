#include <common.h>
#include <memory/paddr.h>

void init_mtrace() { Log("Memory trace initialized"); }

void mtrace_read(paddr_t addr, int len) {
  if (addr >= PMEM_LEFT && addr <= PMEM_RIGHT) {
    Log("Memory read from " FMT_PADDR " length %d", addr, len);
  } else {
    Log("Invalid memory read from " FMT_PADDR " length %d", addr, len);
  }
}

void mtrace_write(paddr_t addr, int len, word_t data) {
  if (addr >= PMEM_LEFT && addr <= PMEM_RIGHT) {
    Log("Memory write to " FMT_PADDR " length %d data 0x%08x", addr, len, data);
  } else {
    Log("Invalid memory write to " FMT_PADDR " length %d data 0x%08x", addr,
        len, data);
  }
}
