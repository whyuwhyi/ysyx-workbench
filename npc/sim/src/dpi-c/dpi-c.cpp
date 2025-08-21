#include <VCore__Dpi.h>
#include <common.h>
#include <cstdint>
#include <defs.h>
#include <svdpi.h>

extern NPCState npc_state;

#define PROBE_PC_INST "TOP.Core.datapathInst.ifuInst.pcInst.simProbePCInst"
#define PROBE_RF_INST "TOP.Core.datapathInst.registerFileInst.simProbeRFInst"

uint32_t get_npc_pc() {
  svScope probe_pc_inst = svGetScopeFromName(PROBE_PC_INST);
  assert(probe_pc_inst != NULL);
  svSetScope(probe_pc_inst);
  return get_pc_value();
}

uint32_t get_npc_reg(int idx) {
  svScope probe_rf_inst = svGetScopeFromName(PROBE_RF_INST);
  assert(probe_rf_inst != NULL);
  svSetScope(probe_rf_inst);
  return get_reg_value(idx);
}

extern "C" void ebreak() {
  npc_state.state = NPC_END;
  npc_state.halt_pc = get_npc_pc();
  npc_state.halt_ret = get_npc_reg(10);
}
