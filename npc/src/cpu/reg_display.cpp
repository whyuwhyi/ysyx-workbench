#include <common.h>
#include <cpu/cpu.h>
#include <svdpi.h>

static const char *regs[] = {
    "zero", "ra", "sp", "gp", "tp",  "t0",  "t1", "t2", "s0", "s1", "a0",
    "a1",   "a2", "a3", "a4", "a5",  "a6",  "a7", "s2", "s3", "s4", "s5",
    "s6",   "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

void npc_reg_display() {
  printf("name\t hex\t\t dec\n");
  
  svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.rf_inst"));
  for (int i = 0; i < 32; i++) {
    uint32_t reg_val = get_reg_value(i);
    printf("%s\t 0x%08x\t %d\n", regs[i], reg_val, reg_val);
  }
  
  svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.pc_inst"));
  uint32_t pc_val = get_pc_value();
  printf("pc\t 0x%08x\t %d\n", pc_val, pc_val);
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
