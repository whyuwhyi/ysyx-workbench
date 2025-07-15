#ifndef __MTRACE_H__
#define __MTRACE_H__

#include <common.h>

#define MAX_MTRACE_LEN 32

typedef struct {
  uint32_t addr;
  uint32_t data;
  uint8_t wmask;
  bool is_write;
} MemTrace;

typedef struct {
  MemTrace traces[MAX_MTRACE_LEN];
  size_t current;
} MTrace;

void init_mtrace();
void mtrace_read(uint32_t addr, uint32_t data);
void mtrace_write(uint32_t addr, uint32_t data, uint8_t wmask);
void mtrace_display();
void log_mem_access(uint32_t addr, uint32_t data, bool is_write, int len);

#endif