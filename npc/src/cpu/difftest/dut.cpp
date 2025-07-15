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

// These functions interface with the RTL via DPI-C
// They will be properly implemented when integrating with Verilator

// DPI-C functions for register access
extern "C" uint32_t get_reg_value(int reg_idx);
extern "C" void set_reg_value(int reg_idx, uint32_t value);
extern "C" uint32_t get_pc_value();
extern "C" void set_pc_value(uint32_t pc);

// NPC interface functions used by difftest
uint32_t get_npc_pc() {
  return get_pc_value();
}

uint32_t get_npc_reg(int idx) {
  if (idx == 0) return 0; // x0 is always 0 in RISC-V
  return get_reg_value(idx);
}

void set_npc_reg(int idx, uint32_t value) {
  if (idx == 0) return; // x0 cannot be written
  set_reg_value(idx, value);
}

void set_npc_pc(uint32_t pc) {
  set_pc_value(pc);
}

// Placeholder implementations for DPI-C functions
// These should be replaced with actual Verilog DPI-C implementations

static uint32_t reg_file[32] = {0};
static uint32_t pc_reg = 0x80000000;

extern "C" uint32_t get_reg_value(int reg_idx) {
  if (reg_idx == 0) return 0; // x0 is always 0
  if (reg_idx < 0 || reg_idx >= 32) {
    printf("Warning: Invalid register index %d\n", reg_idx);
    return 0;
  }
  return reg_file[reg_idx];
}

extern "C" void set_reg_value(int reg_idx, uint32_t value) {
  if (reg_idx == 0) return; // x0 cannot be written
  if (reg_idx < 0 || reg_idx >= 32) {
    printf("Warning: Invalid register index %d\n", reg_idx);
    return;
  }
  reg_file[reg_idx] = value;
}

extern "C" uint32_t get_pc_value() {
  return pc_reg;
}

extern "C" void set_pc_value(uint32_t pc) {
  pc_reg = pc;
}