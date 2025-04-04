#include <common.h>
#include <memory/paddr.h>

void mtrace_read(paddr_t addr, int len) {
  if (addr >= PMEM_LEFT && addr <= PMEM_RIGHT) {
    printf("Read from address " FMT_PADDR " with length %d\n", addr, len);
  } else {
    printf("Invalid read address " FMT_PADDR "\n", addr);
  }
}

void mtrace_write(paddr_t addr, int len) {
  if (addr >= PMEM_LEFT && addr <= PMEM_RIGHT) {
    printf("Write to address " FMT_PADDR " with length %d", addr, len);
  } else {
    printf("Invalid write address " FMT_PADDR "\n", addr);
  }
}
