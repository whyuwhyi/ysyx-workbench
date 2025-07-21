#include <common.h>
#include <cpu/cpu.h>
#include <svdpi.h>
#include <utils.h>

extern NPCState npc_state;

extern "C" void ebreak(uint32_t a0) {
  npc_state.state = NPC_END;
  npc_state.halt_pc = get_pc_value();
  npc_state.halt_ret = a0;
}

extern "C" bool check_ebreak() { return npc_state.state == NPC_END; }
