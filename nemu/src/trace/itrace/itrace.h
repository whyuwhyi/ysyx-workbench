#ifndef __ITRACE_H__
#define __ITRACE_H__

#include <common.h>

void init_itrace();
void itrace_push(paddr_t pc);
void itrace_display();

#endif
