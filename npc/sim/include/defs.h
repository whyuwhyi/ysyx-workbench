#ifndef __DEFS_H__
#define __DEFS_H__

#include <common.h>

uint32_t paddr_read(paddr_t addr, int len);
void paddr_write(paddr_t addr, int len, word_t data);
long load_img(const char *img_file);

void init_device(void);

void serial_putchar(char c);

int timer_get_time(void);

paddr_t get_npc_pc();
word_t get_npc_inst();
word_t get_npc_reg(int reg_idx);

void init_cpu();
void init_cpu_difftest(char *ref_so_file, long img_size);
void npc_cpu_exec(uint64_t n);
void npc_reg_display();

void sim_init(void);
void sim_exit(void);
void single_cycle();

bool is_exit_status_bad(void);

word_t npc_reg_str2val(const char *reg_name, bool *success);

void init_monitor(int argc, char *argv[]);
void monitor_exit(void);
void engine_start();
long load_image(char *img_file);

void init_itrace();
void itrace_push(paddr_t pc, word_t inst, const char *disasm);
void itrace_display();

void init_mtrace();
void mtrace_read(paddr_t addr, int len, word_t data);
void mtrace_write(paddr_t addr, int len, word_t data);

void init_ftrace(const char *elf_path);
void ftrace_call(paddr_t from, paddr_t to);
void ftrace_ret(paddr_t from, paddr_t to);
bool is_fcall(word_t inst);
bool is_fret(word_t inst);

void init_dtrace();
void dtrace_read(paddr_t addr, int len, word_t data, const char *device);
void dtrace_write(paddr_t addr, int len, word_t data, const char *device);

void init_etrace();
void etrace_call(paddr_t epc);
void etrace_ret();

void init_sdb();
void sdb_mainloop();
void watchpoint_check();

void init_difftest(char *ref_so_file, long img_size, int port);
void difftest_step(paddr_t pc);

#endif
