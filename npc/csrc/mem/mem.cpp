#include "mem.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>

static word_t pmem[MAX_MEM_SIZE];

word_t pmem_read(paddr_t addr) {
  assert(addr >= (paddr_t)0x80000000);
  printf("pmem_read: addr = 0x%08x\n", addr);
  addr -= 0x80000000;
  return pmem[addr];
}

void pmem_write(paddr_t addr, word_t data) {
  assert(addr >= (paddr_t)0x80000000);
  printf("pmem_write: addr = 0x%08x, data = 0x%08x\n", addr, data);
  addr -= 0x80000000;
  pmem[addr] = data;
}
