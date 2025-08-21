#ifndef __MEMORY_H__
#define __MEMORY_H__

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

#endif
