#include "Vysyx_25030081_cpu__Dpi.h"
#include <common.h>
#include <cpu/cpu.h>
#include <cstdint>
#include <svdpi.h>

extern NPCState npc_state;

uint32_t get_npc_pc() {
  svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.pc_inst"));
  return get_pc_value();
}

uint32_t get_npc_reg(int idx) {
  svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.rf_inst"));
  return get_reg_value(idx);
}

extern "C" void ebreak() {
  npc_state.state = NPC_END;
  npc_state.halt_pc = get_npc_pc();
  npc_state.halt_ret = get_npc_reg(10);
}
