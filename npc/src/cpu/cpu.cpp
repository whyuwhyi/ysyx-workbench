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

// CPU state management
bool npc_state_stopped = false;
static bool npc_state_running = false;

// External function declarations for Verilator integration
extern void single_cycle();
extern "C" bool check_ebreak();
extern "C" int get_pc_value();

extern void init_itrace();
extern void itrace_push(uint32_t pc, uint32_t inst);

extern void init_mtrace();

extern void init_ftrace();
extern void ftrace_call(uint32_t caller_pc, uint32_t target_pc);
extern void ftrace_ret(uint32_t ret_pc, uint32_t target_pc);
extern bool is_fcall(uint32_t inst);
extern bool is_fret(uint32_t inst);

extern void init_difftest(char *ref_so_file, long img_size, int port);
extern void difftest_step(uint32_t pc, uint32_t npc);

extern void sdb_mainloop();
extern void watchpoint_check();

// Initialize CPU and related systems
void init_cpu() {
  npc_state_stopped = false;
  npc_state_running = false;

  Log("Initializing CPU...");

  // Initialize trace systems
#ifdef CONFIG_ITRACE
  init_itrace();
#endif

#ifdef CONFIG_MTRACE
  init_mtrace();
#endif

#ifdef CONFIG_FTRACE
  init_ftrace();
#endif

  Log("CPU initialized");
}

// Initialize difftest with reference
void init_cpu_difftest(char *ref_so_file, long img_size) {
#ifdef CONFIG_DIFFTEST
  init_difftest(ref_so_file, img_size, 1234);
#endif
}

// Execute one instruction
static void exec_once() {
  // Ensure DPI-C scope is set before calling DPI functions
  svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.pc_inst"));

  uint32_t pc = get_pc_value();

  // Execute one cycle in Verilator
  single_cycle();

  uint32_t npc = get_pc_value();

  // Get instruction data for trace
  uint32_t inst = pmem_read(pc);

#ifdef CONFIG_ITRACE
  itrace_push(pc, inst);
#endif

  // Check for function calls/returns for ftrace
#ifdef CONFIG_FTRACE

  if (is_fcall(inst)) {
    ftrace_call(pc, npc);
  } else if (is_fret(inst)) {
    ftrace_ret(pc, npc);
  }
#endif

  // Run difftest check
#ifdef CONFIG_DIFFTEST
  difftest_step(pc, npc);
#endif

  // Check watchpoints
  watchpoint_check();

  // Check NPC state
  extern NPCState npc_state;
  if (npc_state.state == NPC_END) {
    npc_state_stopped = true;
    npc_state.halt_pc = pc;
  }
}

// Execute N instructions
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
      printf("\n");  // 添加换行分隔
      Log("npc: %s at pc = 0x%08x", 
          npc_state.halt_ret == 0 ? 
          ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) : 
          ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED), 
          npc_state.halt_pc);
      
      // 自动退出程序
      npc_state_running = false;
      npc_state_stopped = true;
      extern void monitor_exit();
      monitor_exit();
    } else {
      Log("NPC execution stopped");
    }
  } else {
    Log("NPC execution completed");
  }
}

// Stop CPU execution
void npc_cpu_stop() {
  npc_state_running = false;
  npc_state_stopped = true;
}

// Get CPU state
bool npc_is_stopped() { return npc_state_stopped; }

bool npc_is_running() { return npc_state_running; }
