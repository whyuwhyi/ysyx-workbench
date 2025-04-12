#include "inst.h"

void addi(paddr_t addr, word_t rs1, word_t imm, word_t rd) {
  word_t inst = imm << 20 | rs1 << 15 | rd << 7 | 0x13;
  pmem_write(addr, inst);
}

void ebreak(paddr_t addr) {
  word_t inst = 0x00100073;
  pmem_write(addr, inst);
}
