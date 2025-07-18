#ifndef __ITRACE_H__
#define __ITRACE_H__

#include <common.h>

#define MAX_TRACE_LEN 24

typedef struct {
  uint32_t inst_addr[MAX_TRACE_LEN];
  uint32_t inst_data[MAX_TRACE_LEN];
  size_t current;
} InstTrace;

void init_itrace();
void itrace_push(uint32_t pc, uint32_t inst);
void itrace_display();

#endif
