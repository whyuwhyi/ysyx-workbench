#ifndef __MTRACE_H__
#define __MTRACE_H__

#include <common.h>

void init_mtrace();
void mtrace_read(paddr_t addr, int len);
void mtrace_write(paddr_t addr, int len, word_t data);

#endif