#include <common.h>
#include <cpu/cpu.h>
#include <svdpi.h>

static const char *regs[] = {
    "zero", "ra", "sp", "gp", "tp",  "t0",  "t1", "t2", "s0", "s1", "a0",
    "a1",   "a2", "a3", "a4", "a5",  "a6",  "a7", "s2", "s3", "s4", "s5",
    "s6",   "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

void npc_reg_display() {
  svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.pc_inst"));
  uint32_t pc_val = get_pc_value();

  printf("Register values:\n");
  printf("PC: 0x%08x\n", pc_val);

  svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.rf_inst"));
  for (int i = 0; i < 32; i += 4) {
    printf("x%-2d(%-4s): 0x%08x  x%-2d(%-4s): 0x%08x  x%-2d(%-4s): 0x%08x  "
           "x%-2d(%-4s): 0x%08x",
           i, regs[i], get_reg_value(i), i + 1, regs[i + 1],
           get_reg_value(i + 1), i + 2, regs[i + 2], get_reg_value(i + 2),
           i + 3, regs[i + 3], get_reg_value(i + 3));
  }
}

// Get register value by name (for expression evaluator)
uint32_t npc_reg_str2val(const char *reg_name, bool *success) {
  *success = true;

  for (int i = 0; i < 32; i++) {
    if (strcmp(reg_name, regs[i]) == 0) {
      svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.rf_inst"));
      return get_reg_value(i);
    }
  }

  if (reg_name[0] == 'x' && strlen(reg_name) >= 2) {
    int reg_idx = atoi(reg_name + 1);
    if (reg_idx >= 0 && reg_idx < 32) {
      svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.rf_inst"));
      return get_reg_value(reg_idx);
    }
  }

  if (strcmp(reg_name, "pc") == 0) {
    svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.pc_inst"));
    return get_pc_value();
  }

  Log("Unknown register: %s", reg_name);
  *success = false;
  return 0;
}
