#include <common.h>
#include <cpu/cpu.h>
#include <utils.h>
#include <svdpi.h>


extern "C" void ebreak(uint32_t a0) {
  extern NPCState npc_state;
  npc_state.state = NPC_END;
  npc_state.halt_pc = get_pc_value();
  npc_state.halt_ret = a0;
}

extern "C" bool check_ebreak() {
  extern NPCState npc_state;
  return npc_state.state == NPC_END;
}

