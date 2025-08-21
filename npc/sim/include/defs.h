#ifndef __DEFS_H__
#define __DEFS_H__

#include <common.h>

extern "C" {
int get_pc_value();
int get_reg_value(int reg_idx);
void ebreak();
int pmem_read(int raddr);
void pmem_write(int waddr, int wdata, char wmask);
}

uint32_t paddr_read(uint32_t addr, int len);
void paddr_write(uint32_t addr, int len, uint32_t data);
long load_img(const char *img_file);
void init_mem();

void init_device(void);
void serial_init(void);
void timer_init(void);

void serial_putchar(char c);

int timer_get_time(void);

uint32_t get_npc_pc();
uint32_t get_npc_reg(int reg_idx);

void init_cpu();
void init_cpu_difftest(char *ref_so_file, long img_size);
void npc_cpu_exec(uint64_t n);
void npc_reg_display();

void sim_init(void);
void sim_exit(void);
void single_cycle();

bool is_exit_status_bad(void);

uint32_t npc_reg_str2val(const char *reg_name, bool *success);

void init_monitor(int argc, char *argv[]);
void monitor_exit(void);
void engine_start();
long load_image(char *img_file);

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

void init_sdb();
void sdb_mainloop();
void watchpoint_check();

void init_difftest(char *ref_so_file, long img_size, int port);
void difftest_step(uint32_t pc, uint32_t npc);

#endif
