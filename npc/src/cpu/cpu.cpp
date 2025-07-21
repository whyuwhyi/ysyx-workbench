/***************************************************************************************
 * Copyright (c) 2014-2024 Zihao Yu, Nanjing University
 *
 * NPC is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan
 *PSL v2. You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
 *KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 *NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include <common.h>
#include <cpu/cpu.h>
#include <memory/pmem.h>
#include <svdpi.h>

bool npc_state_stopped = false;
static bool npc_state_running = false;

void init_cpu() {
  npc_state_stopped = false;
  npc_state_running = false;
  Log("CPU initialized");
}

void init_cpu_difftest(char *ref_so_file, long img_size) {
#ifdef CONFIG_DIFFTEST
  init_difftest(ref_so_file, img_size, 1234);
#endif
}

static void exec_once() {
  svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.pc_inst"));

  uint32_t pc = get_pc_value();

  single_cycle();

  uint32_t npc = get_pc_value();

  uint32_t inst = pmem_read(pc);

#ifdef CONFIG_ITRACE
  itrace_push(pc);
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

  extern NPCState npc_state;
  if (npc_state.state == NPC_END) {
    npc_state_stopped = true;
    npc_state.halt_pc = pc;
  }
}

void npc_cpu_exec(uint64_t n) {
  npc_state_running = true;
  npc_state_stopped = false;

  if (n == (uint64_t)-1) {
    Log("NPC started (continuous execution)");
    while (!npc_state_stopped && npc_state_running) {
      exec_once();
    }
  } else {
    Log("NPC executing %lu instructions", n);
    for (uint64_t i = 0; i < n && !npc_state_stopped; i++) {
      exec_once();
    }
  }

  npc_state_running = false;

  if (npc_state_stopped) {
    extern NPCState npc_state;
    if (npc_state.state == NPC_END) {
      Log("npc: %s at pc = 0x%08x",
          (npc_state.halt_ret == 0
               ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN)
               : ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED)),
          npc_state.halt_pc);
      npc_state_running = false;
      npc_state_stopped = true;
    } else {
      Log("NPC execution stopped");
    }
  } else {
    Log("NPC execution completed");
  }
}

void npc_cpu_stop() {
  npc_state_running = false;
  npc_state_stopped = true;
}

bool npc_is_stopped() { return npc_state_stopped; }

bool npc_is_running() { return npc_state_running; }
