#include <VCore__Dpi.h>
#include <common.h>
#include <cstdint>
#include <defs.h>
#include <svdpi.h>

extern NPCState npc_state;

#define SIM_IMEM_INST "TOP.Core.datapathInst.ifuInst.simIMemInst"
#define PROBE_RF_INST "TOP.Core.datapathInst.registerFileInst.simProbeRFInst"

extern "C" {
int get_pc_value();
int get_reg_value(int reg_idx);
int get_inst_value();
void ebreak();
int pmem_read(int raddr);
void pmem_write(int waddr, int wdata, char wmask);
}

uint32_t get_npc_pc() {
  svScope sim_imem_inst = svGetScopeFromName(SIM_IMEM_INST);
  assert(sim_imem_inst != NULL);
  svSetScope(sim_imem_inst);
  return get_pc_value();
}

uint32_t get_npc_inst() {
  svScope sim_imem_inst = svGetScopeFromName(SIM_IMEM_INST);
  assert(sim_imem_inst != NULL);
  svSetScope(sim_imem_inst);
  return get_inst_value();
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
