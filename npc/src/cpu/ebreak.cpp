#include "common.h"
#include "cpu/cpu.h"
#include "cpu/simulator.h"

bool is_sim_end = false;
NPCState npc_state_info = {NPC_RUNNING, 0, 0};

extern "C" void ebreak(void) {
  uint32_t pc = get_pc();
  int halt_ret = get_gpr(10); // a0寄存器，RISC-V中a0是x10

  set_npc_state(NPC_END, pc, halt_ret);
  is_sim_end = true;
}

extern "C" void set_npc_state(int state, uint32_t pc, int halt_ret) {
  npc_state_info.state = state;
  npc_state_info.halt_pc = pc;
  npc_state_info.halt_ret = halt_ret;
}

extern "C" bool is_exit_status_bad(void) {
  return (npc_state_info.state == NPC_END && npc_state_info.halt_ret != 0) ||
         (npc_state_info.state == NPC_ABORT);
}
