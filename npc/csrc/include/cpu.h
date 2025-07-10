#ifndef __CPU_H__
#define __CPU_H__

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

void ebreak(void);
void set_npc_state(npc_state_t state, uint32_t pc, int halt_ret);
bool is_exit_status_bad(void);

#ifdef __cplusplus
}
#endif

#endif