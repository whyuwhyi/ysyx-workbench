#include "common.h"

static void print_trap_info() {
  switch (npc_state_info.state) {
  case NPC_END:
    if (npc_state_info.halt_ret == 0) {
      printf(ANSI_FG_GREEN "npc: HIT GOOD TRAP at pc = 0x%08x" ANSI_NONE "\n",
             npc_state_info.halt_pc);
    } else {
      printf(ANSI_FG_RED "npc: HIT BAD TRAP at pc = 0x%08x" ANSI_NONE "\n",
             npc_state_info.halt_pc);
    }
    break;
  case NPC_ABORT:
    printf(ANSI_FG_RED "npc: ABORT at pc = 0x%08x" ANSI_NONE "\n",
           npc_state_info.halt_pc);
    break;
  default:
    break;
  }
}

int main(int argc, char **argv) {
  monitor_init();
  sim_init();

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <image_file>\n", argv[0]);
    return -1;
  }

  long size = load_image(argv[1]);

  while (!is_sim_end) {
    run(1);
  }

  print_trap_info();

  sim_exit();
  monitor_exit();

  return is_exit_status_bad() ? 1 : 0;
}
