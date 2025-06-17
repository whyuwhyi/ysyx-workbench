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
#include <cpu/difftest.h>
#include <isa.h>

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  if (cpu.pc != ref_r->pc) {
    Log("PC mismatch: DUT = " FMT_WORD ", REF = " FMT_WORD, pc, ref_r->pc);
    return false;
  }
  bool diff = false;
  for (int i = 0; i < RISCV_GPR_NUM; i++) {
    if (!difftest_check_reg(reg_name(i), pc, ref_r->gpr[i], cpu.gpr[i])) {
      diff = true;
    }
  }
  return !diff;
}

void isa_difftest_attach() {}
