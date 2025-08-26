/***************************************************************************************
 * Copyright (c) 2014-2024 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan
 *PSL v2. You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
 *KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 *NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include "local-include/reg.h"
#include "common.h"
#include <isa.h>

const char *regs[] = {"$0", "ra", "sp",  "gp",  "tp", "t0", "t1", "t2",
                      "s0", "s1", "a0",  "a1",  "a2", "a3", "a4", "a5",
                      "a6", "a7", "s2",  "s3",  "s4", "s5", "s6", "s7",
                      "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

const char *csrs[] = {[MSTATUS] = "mstatus",
                      [MTVEC] = "mtvec",
                      [MEPC] = "mepc",
                      [MCAUSE] = "mcause"};

static inline void riscv_csr_display(int addr) {
  printf("%-11s %-11d " FMT_WORD "\n", csrs[addr], (sword_t)csr(addr),
         csr(addr));
}

void isa_reg_display() {
  printf("Name        Dec         Hex        \n");
  printf("-----General Purpose Registers-----\n");
  for (int i = 0; i < 32; ++i) {
    printf("%-11s %-11d " FMT_WORD "\n", regs[i], (sword_t)gpr(i), gpr(i));
  }

  printf("----Control and Status Registers----\n");
  riscv_csr_display(MSTATUS);
  riscv_csr_display(MTVEC);
  riscv_csr_display(MEPC);
  riscv_csr_display(MCAUSE);

  printf("----------Program Counter-----------\n");
  printf("%-11s %-11d " FMT_WORD "\n", "pc", (sword_t)cpu.pc, cpu.pc);
}

#define CSR2VAL(addr)                                                          \
  if (strcmp(csrs[addr], s) == 0) {                                            \
    *success = true;                                                           \
    return csr(addr);                                                          \
  }

word_t isa_reg_str2val(const char *s, bool *success) {
  for (int i = 0; i < 32; ++i) {
    if (strcmp(regs[i], s) == 0) {
      *success = true;
      return gpr(i);
    }
  }

  CSR2VAL(MSTATUS)
  CSR2VAL(MTVEC)
  CSR2VAL(MEPC)
  CSR2VAL(MCAUSE)

  if (strcmp("pc", s) == 0) {
    *success = true;
    return cpu.pc;
  }

  *success = false;
  return 0;
}
