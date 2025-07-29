#include "Vysyx_25030081_cpu__Dpi.h"
#include <common.h>
#include <cpu/cpu.h>
#include <svdpi.h>

extern NPCState npc_state;

extern "C" void ebreak() {
  npc_state.state = NPC_END;
  svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.pc_inst"));
  npc_state.halt_pc = get_pc_value();
  svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.rf_inst"));
  npc_state.halt_ret = get_reg_value(10);
}

uint32_t get_pc_value() {
  svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.pc_inst"));
  return get_npc_pc();
}

uint32_t get_reg_value(int reg_idx) {
  svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.rf_inst"));
  return get_npc_reg(reg_idx);
}
