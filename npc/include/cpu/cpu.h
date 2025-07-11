#ifndef __CPU_H__
#define __CPU_H__

#include "common.h"
#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

void ebreak(void);
void set_npc_state(int state, uint32_t pc, int halt_ret);
bool is_exit_status_bad(void);

#ifdef __cplusplus
}
#endif

#endif