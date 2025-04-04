#ifndef __ITRACE_H__
#define __ITRACE_H__

#include <common.h>

#define MAX_TRACE_LEN 24
typedef struct {
  vaddr_t inst_addr[MAX_TRACE_LEN];
  size_t current;
} InstTrace;

#endif
