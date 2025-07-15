#include <common.h>
#include <cpu/cpu.h>

// RISC-V register names
static const char *regs[] = {
    "zero", "ra", "sp", "gp", "tp",  "t0",  "t1", "t2", "s0", "s1", "a0",
    "a1",   "a2", "a3", "a4", "a5",  "a6",  "a7", "s2", "s3", "s4", "s5",
    "s6",   "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

// Display all registers
void npc_reg_display() {
  printf("Register values:\n");
  printf("PC: 0x%08x\n", get_pc_value());
  printf("\n");

  for (int i = 0; i < 32; i += 4) {
    printf("x%-2d(%-4s): 0x%08x  ", i, regs[i], get_reg_value(i));
    printf("x%-2d(%-4s): 0x%08x  ", i + 1, regs[i + 1], get_reg_value(i + 1));
    printf("x%-2d(%-4s): 0x%08x  ", i + 2, regs[i + 2], get_reg_value(i + 2));
    printf("x%-2d(%-4s): 0x%08x\n", i + 3, regs[i + 3], get_reg_value(i + 3));
  }
  printf("\n");
}

// Get register value by name (for expression evaluator)
uint32_t npc_reg_str2val(const char *reg_name, bool *success) {
  *success = true;

  // Check for register names
  for (int i = 0; i < 32; i++) {
    if (strcmp(reg_name, regs[i]) == 0) {
      return get_reg_value(i);
    }
  }

  // Check for x0-x31 format
  if (reg_name[0] == 'x' && strlen(reg_name) >= 2) {
    int reg_idx = atoi(reg_name + 1);
    if (reg_idx >= 0 && reg_idx < 32) {
      return get_reg_value(reg_idx);
    }
  }

  // Check for special registers
  if (strcmp(reg_name, "pc") == 0) {
    return get_pc_value();
  }

  printf("Unknown register: %s\n", reg_name);
  *success = false;
  return 0;
}
