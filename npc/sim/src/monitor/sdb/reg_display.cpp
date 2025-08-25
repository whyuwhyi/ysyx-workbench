#include <defs.h>
#include <svdpi.h>

static const char *regs[] = {
    "zero", "ra", "sp", "gp", "tp",  "t0",  "t1", "t2", "s0", "s1", "a0",
    "a1",   "a2", "a3", "a4", "a5",  "a6",  "a7", "s2", "s3", "s4", "s5",
    "s6",   "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

void npc_reg_display() {
  printf("Name Dec         Hex        \n");

  for (int i = 0; i < 32; i++) {
    uint32_t reg_val = get_npc_reg(i);
    printf("%-4s %-11d 0x%08x\n", regs[i], (int32_t)reg_val, reg_val);
  }

  uint32_t pc_val = get_npc_pc();
  printf("%-4s %-11d 0x%08x\n", "pc", (int32_t)pc_val, pc_val);
}

uint32_t npc_reg_str2val(const char *reg_name, bool *success) {
  *success = true;

  for (int i = 0; i < 32; i++) {
    if (strcmp(reg_name, regs[i]) == 0) {
      return get_npc_reg(i);
    }
  }

  if (reg_name[0] == 'x' && strlen(reg_name) >= 2) {
    int reg_idx = atoi(reg_name + 1);
    if (reg_idx >= 0 && reg_idx < 32) {
      return get_npc_reg(reg_idx);
    }
  }

  if (strcmp(reg_name, "pc") == 0) {
    return get_npc_pc();
  }

  Log("Unknown register: %s", reg_name);
  *success = false;
  return 0;
}
