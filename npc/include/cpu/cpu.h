#ifndef __CPU_H__
#define __CPU_H__

#include "common.h"
#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

// Legacy ebreak support
void ebreak(void);
void set_npc_state(int state, uint32_t pc, int halt_ret);
bool is_exit_status_bad(void);

// DPI-C interface functions (implemented in RTL)
#ifdef __cplusplus
extern "C" {
#endif
int get_pc_value();
int get_reg_value(int reg_idx);
#ifdef __cplusplus
}
#endif

// Verilator integration functions
void single_cycle();
bool check_ebreak();

#ifdef __cplusplus
}
#endif

// C++ CPU interface functions
void init_cpu();
void init_cpu_difftest(char *ref_so_file, long img_size);
void npc_cpu_exec(uint64_t n);
void npc_cpu_stop();
void npc_cpu_reset();
bool npc_is_stopped();
bool npc_is_running();

// Register display function
void npc_reg_display();

// CPU state globals
extern bool npc_state_stopped;

// NPC state access functions for difftest
uint32_t get_npc_pc();
uint32_t get_npc_reg(int idx);

#endif
