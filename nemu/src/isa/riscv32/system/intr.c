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

#include "../local-include/reg.h"
#include <isa.h>

void etrace_call(paddr_t epc);
void etrace_ret();

vaddr_t isa_raise_intr(word_t NO, vaddr_t epc) {
  csr(MEPC) = epc;
  csr(MCAUSE) = NO;
  csr(MSTATUS) = 0x80;

  IFDEF(CONFIG_ETRACE, etrace_call(epc));
  return csr(MTVEC);
}

vaddr_t isa_return_intr() {
  IFDEF(CONFIG_ETRACE, etrace_ret());
  csr(MSTATUS) = 0x80;
  return csr(MEPC);
}

word_t isa_query_intr() { return INTR_EMPTY; }
