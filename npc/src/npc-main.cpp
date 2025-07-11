#include "common.h"
#include "cpu/simulator.h"

// Function declarations
void monitor_init(void);
void monitor_exit(void);
long load_image(char *img_file);
int is_exit_status_bad(void);
extern bool is_sim_end;

static void print_trap_info() {
  switch (npc_state.state) {
  case NPC_END:
    if (npc_state.halt_ret == 0) {
      printf(ANSI_FMT("npc: HIT GOOD TRAP at pc = 0x%08x", ANSI_FG_GREEN) "\n",
             npc_state.halt_pc);
    } else {
      printf(ANSI_FMT("npc: HIT BAD TRAP at pc = 0x%08x", ANSI_FG_RED) "\n",
             npc_state.halt_pc);
    }
    break;
  case NPC_ABORT:
    printf(ANSI_FMT("npc: ABORT at pc = 0x%08x", ANSI_FG_RED) "\n",
           npc_state.halt_pc);
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
