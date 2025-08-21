#include <common.h>
#include <defs.h>
#include <memory/memory.h>
#include <svdpi.h>

static bool g_print_step = false;

static void exec_once() {
  uint32_t pc = get_npc_pc();
  uint32_t inst = pmem_read(pc);

  single_cycle();

  uint32_t npc = get_npc_pc();

#ifdef CONFIG_ITRACE
  char logbuf[128];
  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  disassemble(logbuf, sizeof(logbuf), pc, (uint8_t *)&inst, 4);

  if (g_print_step) {
    printf("0x%08x:\t%08x:\t%s\n", pc, inst, logbuf);
  }

  itrace_push(pc, inst, logbuf);
#endif

#ifdef CONFIG_FTRACE

  if (is_fcall(inst)) {
    ftrace_call(pc, npc);
  } else if (is_fret(inst)) {
    ftrace_ret(pc, npc);
  }
#endif

#ifdef CONFIG_DIFFTEST
  difftest_step(pc, npc);
#endif

  watchpoint_check();
}

void npc_cpu_exec(uint64_t n) {
  g_print_step = (n < 10);
  npc_state.state = NPC_RUNNING;

  if (n == (uint64_t)-1) {
    Log("NPC started (continuous execution)");
    g_print_step = false;
    while (npc_state.state == NPC_RUNNING) {
      exec_once();
    }
  } else {
    Log("NPC executing %lu instructions", n);
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
