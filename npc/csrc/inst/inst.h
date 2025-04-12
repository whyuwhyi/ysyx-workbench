#ifndef __INST_H__
#define __INST_H__

#include "../mem/mem.h"

void addi(paddr_t addr, word_t rs1, word_t imm, word_t rd);
void ebreak(paddr_t addr);

#endif
