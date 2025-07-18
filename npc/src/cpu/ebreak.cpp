#include <common.h>
#include <cpu/cpu.h>
#include <cpu/simulator.h>

bool ebreak_flag = false;
NPCState npc_state_info = {NPC_RUNNING, 0, 0};

extern "C" void ebreak(void) {
  // set_npc_state(NPC_END, pc, halt_ret);
  ebreak_flag = true;
  Log("EBREAK instruction executed");
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

extern "C" bool check_ebreak() {
  return ebreak_flag;
}
