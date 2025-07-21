#include <common.h>
#include <cpu/cpu.h>
#include <svdpi.h>

uint32_t get_npc_pc() {
  svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.pc_inst"));
  return get_pc_value();
}

uint32_t get_npc_reg(int idx) {
  if (idx == 0)
    return 0; // x0 is always 0 in RISC-V
  svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.rf_inst"));
  return get_reg_value(idx);
}
