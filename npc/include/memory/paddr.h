#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "common.h"

#define MEM_SIZE (0x10000000)
#define MEM_BASE (0x80000000)

#ifdef __cplusplus
extern "C" {
#endif

int pmem_read(int raddr);
void pmem_write(int waddr, int wdata, char wmask);
uint8_t* get_pmem();

#ifdef __cplusplus
}
#endif

#endif