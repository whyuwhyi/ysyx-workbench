#ifndef __FTRACE_H__
#define __FTRACE_H__

#include <common.h>

#define MAX_FTRACE_LEN 32
#define MAX_FUNC_NAME 64
#define MAX_FUNC 1024

typedef struct {
  uint32_t addr;
  uint32_t size;
  const char *name;
} FuncInfo;

typedef struct {
  uint32_t caller_pc;
  uint32_t target_pc;
  char func_name[MAX_FUNC_NAME];
  bool is_call;
  int depth;
} FuncTrace;

typedef struct {
  FuncTrace traces[MAX_FTRACE_LEN];
  size_t current;
  int call_depth;
} FTrace;

void init_ftrace();
void init_ftrace_elf(const char *elf_path);
void ftrace_call(uint32_t caller_pc, uint32_t target_pc);
void ftrace_ret(uint32_t ret_pc, uint32_t target_pc);
void ftrace_display();
const char *ftrace_func_name(uint32_t addr);
bool is_fcall(uint32_t inst);
bool is_fret(uint32_t inst);

#endif