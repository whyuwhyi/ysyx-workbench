#ifndef __CPU_H__
#define __CPU_H__

#include "common.h"
#include "utils.h"

enum { NPC_RUNNING, NPC_STOP, NPC_END, NPC_ABORT, NPC_QUIT };

typedef struct {
  int state;
  vaddr_t halt_pc;
  uint32_t halt_ret;
} NPCState;

extern NPCState npc_state;

uint64_t get_time();

// DPI-C interface functions
extern "C" {
uint32_t get_pc_value();
uint32_t get_reg_value(int reg_idx);
void ebreak();
bool check_ebreak();
uint32_t pmem_read(uint32_t raddr);
void pmem_write(uint32_t waddr, uint32_t wdata, char wmask);
}

uint32_t get_npc_pc();
uint32_t get_npc_reg(int reg_idx);

// C++ CPU interface functions
void init_cpu();
void init_cpu_difftest(char *ref_so_file, long img_size);
void npc_cpu_exec(uint64_t n);
void npc_cpu_stop();
void npc_cpu_reset();
bool npc_is_stopped();
void npc_reg_display();

// Simulator functions
void sim_init(void);
void sim_exit(void);
void single_cycle();

// State management functions
void set_npc_state(int state, uint32_t pc, int halt_ret);
bool is_exit_status_bad(void);
bool npc_is_running();

// CPU state globals
extern bool npc_state_stopped;

// Register access functions
uint32_t npc_reg_str2val(const char *reg_name, bool *success);

// Monitor and system functions
void init_monitor(int argc, char *argv[]);
void monitor_exit(void);
void engine_start();
long load_image(char *img_file);

// Trace functions
void init_itrace();
void init_mtrace();
void mtrace_read(uint32_t addr, int len);
void mtrace_write(uint32_t addr, int len, uint32_t data);
void init_ftrace(const char *elf_path);
void itrace_push(uint32_t pc, uint32_t inst);
void itrace_display();
void ftrace_call(uint32_t caller_pc, uint32_t target_pc);
void ftrace_ret(uint32_t ret_pc, uint32_t target_pc);
bool is_fcall(uint32_t inst);
bool is_fret(uint32_t inst);

// Debugger functions
void init_sdb();
void sdb_mainloop();
void sdb_set_batch_mode();
void watchpoint_check();

// Difftest functions
void init_difftest(char *ref_so_file, long img_size, int port);
void difftest_step(uint32_t pc, uint32_t npc);

#endif
