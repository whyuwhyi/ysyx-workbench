#ifndef __FTRACE_H__
#define __FTRACE_H__
#include <common.h>

#define MAX_FUNC 1024

typedef struct {
  paddr_t addr;
  word_t size;
  const char *name;
} FuncInfo;

void init_ftrace(const char *elf_path);
void ftrace_call(vaddr_t from, vaddr_t to);
void ftrace_ret(vaddr_t from, vaddr_t to);
const char *ftrace_func_name(vaddr_t addr);
bool is_fcall(uint32_t inst);
bool is_fret(uint32_t inst);

#endif
