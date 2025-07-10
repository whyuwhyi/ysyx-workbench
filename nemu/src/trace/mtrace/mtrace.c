#include <common.h>
#include <memory/paddr.h>

void mtrace_read(paddr_t addr, int len) {
  if (addr >= PMEM_LEFT && addr <= PMEM_RIGHT) {
    printf("Memory read from " FMT_PADDR " length %d\n", addr, len);
  } else {
    printf("Invalid memory read from " FMT_PADDR " length %d\n", addr, len);
  }
}

void mtrace_write(paddr_t addr, int len, word_t data) {
  if (addr >= PMEM_LEFT && addr <= PMEM_RIGHT) {
    printf("Memory write to " FMT_PADDR " length %d data 0x%08x\n", addr, len, data);
  } else {
    printf("Invalid memory write to " FMT_PADDR " length %d data 0x%08x\n", addr, len, data);
  }
}

void init_mtrace() {
  printf("Memory trace initialized\n");
}
