#ifndef __MEM_H__
#define __MEM_H__
#include <stdint.h>

typedef uint32_t word_t;
typedef uint32_t paddr_t;

#define MAX_MEM_SIZE (0x10000000)

void pmem_init();
word_t pmem_read(paddr_t addr);
void pmem_write(paddr_t addr, word_t data);
void pmem_destory();

#endif
