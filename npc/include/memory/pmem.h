#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "common.h"

#ifndef CONFIG_MSIZE
#define MEM_SIZE (0x10000000)
#else
#define MEM_SIZE CONFIG_MSIZE
#endif

#ifndef CONFIG_MBASE
#define MEM_BASE (0x80000000)
#else
#define MEM_BASE CONFIG_MBASE
#endif

uint8_t *get_pmem();

uint32_t paddr_read(uint32_t addr, int len);
void paddr_write(uint32_t addr, int len, uint32_t data);
long load_img(const char *img_file);
void init_mem();

#endif
