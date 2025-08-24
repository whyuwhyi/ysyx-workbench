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

#ifndef __RISCV_REG_H__
#define __RISCV_REG_H__

#include <common.h>
#include <isa.h>

static inline int check_reg_idx(int idx) {
  IFDEF(CONFIG_RT_CHECK, assert(idx >= 0 && idx < MUXDEF(CONFIG_RVE, 16, 32)));
  return idx;
}

#define gpr(idx) (cpu.gpr[check_reg_idx(idx)])

static inline const char *reg_name(int idx) {
  extern const char *regs[];
  return regs[check_reg_idx(idx)];
}

#define MSTATUS 0x300
#define MTVEC 0x305
#define MEPC 0x341
#define MCAUSE 0x342

static inline word_t *get_csr_ptr(int addr) {
  switch (addr) {
  case MSTATUS:
    return (word_t *)&cpu.csr.mstatus;
  case MTVEC:
    return &cpu.csr.mtvec;
  case MEPC:
    return &cpu.csr.mepc;
  case MCAUSE:
    return &cpu.csr.mcause;
  default:
    Assert(0, "Unimplemented CSR address: 0x%x", addr);
    return NULL;
  }
}

#define csr(i) (*get_csr_ptr(i))

static inline const char *csr_name(int addr) {
  extern const char *csrs[];
  return csrs[addr];
}
#endif
