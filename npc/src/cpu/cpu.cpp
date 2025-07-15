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

// Forward declarations for trace functions
#ifdef CONFIG_ITRACE
extern void itrace_push(uint32_t pc, uint32_t inst);
#endif

#ifdef CONFIG_FTRACE
extern void ftrace_call(uint32_t pc, uint32_t target, const char *func_name);
extern void ftrace_return(uint32_t pc, const char *func_name);
#endif

// Forward declaration for SDB
extern void sdb_mainloop();

// CPU state management
bool npc_state_stopped = false;
static bool npc_state_running = false;

// External function declarations for Verilator integration
extern void single_cycle();
extern void reset_npc();
extern bool check_ebreak();

// Trace system declarations
#ifdef CONFIG_ITRACE
extern void init_itrace();
extern void itrace_push(uint32_t pc, uint32_t inst);
#endif

#ifdef CONFIG_MTRACE
extern void init_mtrace();
#endif

#ifdef CONFIG_FTRACE
extern void init_ftrace();
extern void ftrace_call(uint32_t caller_pc, uint32_t target_pc);
extern void ftrace_ret(uint32_t ret_pc, uint32_t target_pc);
#endif

#ifdef CONFIG_DIFFTEST
extern void init_difftest(char *ref_so_file, long img_size, int port);
extern void difftest_step(uint32_t pc, uint32_t npc);
#endif

#ifdef CONFIG_WATCHPOINT
extern void watchpoint_check();
#endif

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
  uint32_t pc = get_pc_value();

  // Execute one cycle in Verilator
  single_cycle();

  uint32_t npc = get_pc_value();

  // Get instruction data for trace
#ifdef CONFIG_ITRACE
#endif

  // Check for function calls/returns for ftrace
#ifdef CONFIG_FTRACE
#endif

  // Run difftest check
#ifdef CONFIG_DIFFTEST
  difftest_step(pc, npc);
#endif

  // Check watchpoints
#ifdef CONFIG_WATCHPOINT
  watchpoint_check();
#endif

  // Check for ebreak
  if (check_ebreak()) {
    npc_state_stopped = true;
    Log("NPC stopped at ebreak instruction at pc = 0x%08x", npc);
  }
}

// Execute N instructions
void npc_cpu_exec(uint64_t n) {
  npc_state_running = true;
  npc_state_stopped = false;

  if (n == (uint64_t)-1) {
    // Run until stopped
    Log("NPC started (continuous execution)");
    while (!npc_state_stopped && npc_state_running) {
      exec_once();
    }
  } else {
    // Run for N instructions
    Log("NPC executing %lu instructions", n);
    for (uint64_t i = 0; i < n && !npc_state_stopped; i++) {
      exec_once();
    }
  }

  npc_state_running = false;

  if (npc_state_stopped) {
    Log("NPC execution stopped");
  } else {
    Log("NPC execution completed");
  }
}

// Stop CPU execution
void npc_cpu_stop() {
  npc_state_running = false;
  npc_state_stopped = true;
}

// Reset CPU
void npc_cpu_reset() {
  Log("Resetting NPC...");
  reset_npc();
  npc_state_stopped = false;
  npc_state_running = false;
}

// Get CPU state
bool npc_is_stopped() { return npc_state_stopped; }

bool npc_is_running() { return npc_state_running; }
