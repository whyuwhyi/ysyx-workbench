#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

void single_cycle(void);
void run(int n);
void sim_init(void);
void sim_exit(void);
uint32_t get_pc(void);
int get_gpr(int idx);

#ifdef __cplusplus
}
#endif

#endif