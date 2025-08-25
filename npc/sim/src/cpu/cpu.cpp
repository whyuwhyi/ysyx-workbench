#include <defs.h>
#include <memory/memory.h>
#include <svdpi.h>

static void exec_once() {
  uint32_t pc = get_npc_pc();
  uint32_t inst = get_npc_inst();

#ifdef CONFIG_ITRACE
  char logbuf[128];
  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  disassemble(logbuf, sizeof(logbuf), pc, (uint8_t *)&inst, 4);

  printf("0x%08x:\t%08x:\t%s\n", pc, inst, logbuf);

  itrace_push(pc, inst, logbuf);
#endif

  single_cycle();

#ifdef CONFIG_FTRACE

  uint32_t npc = get_npc_pc();

  if (is_fcall(inst)) {
    ftrace_call(pc, npc);
  } else if (is_fret(inst)) {
    ftrace_ret(pc, npc);
  }
#endif

#ifdef CONFIG_DIFFTEST
  difftest_step(pc);
#endif

  watchpoint_check();
}

void npc_cpu_exec(uint64_t n) {
  switch (npc_state.state) {
  case NPC_END:
  case NPC_ABORT:
  case NPC_QUIT:
    printf("Program execution has ended.\n");
    return;
  default:
    npc_state.state = NPC_RUNNING;
  }

  if (n == (uint64_t)-1) {
    while (npc_state.state == NPC_RUNNING) {
      exec_once();
    }
  } else {
    for (uint64_t i = 0; i < n && npc_state.state == NPC_RUNNING; i++) {
      exec_once();
    }
  }

  extern NPCState npc_state;
  switch (npc_state.state) {
  case NPC_RUNNING:
    npc_state.state = NPC_STOP;
    break;

  case NPC_END:
  case NPC_ABORT:
    Log("npc: %s at pc = 0x%08x",
        (npc_state.state == NPC_ABORT
             ? ANSI_FMT("ABORT", ANSI_FG_RED)
             : (npc_state.halt_ret == 0
                    ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN)
                    : ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
        npc_state.halt_pc);
#ifdef CONFIG_ITRACE
    if (npc_state.halt_ret != 0) {
      itrace_display();
    }
#endif
    break;
  case NPC_QUIT:
    break;
  }
}
