#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

void single_cycle(void);
void sim_init(void);
void sim_exit(void);

#ifdef __cplusplus
}
#endif

#endif
