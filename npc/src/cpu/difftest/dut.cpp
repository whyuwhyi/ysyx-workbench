/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NPC is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <common.h>
#include <cpu/cpu.h>
#include <svdpi.h>

// These functions interface with the RTL via DPI-C
// They will be properly implemented when integrating with Verilator


// NPC interface functions used by difftest
uint32_t get_npc_pc() {
  svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.pc_inst"));
  return get_pc_value();
}

uint32_t get_npc_reg(int idx) {
  if (idx == 0) return 0; // x0 is always 0 in RISC-V
  svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.rf_inst"));
  return get_reg_value(idx);
}


// DPI-C functions are now implemented in RTL (rf.v and pc.v)
