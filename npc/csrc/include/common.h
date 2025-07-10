#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define NR_INST 100

#define ANSI_FG_RED     "\33[1;31m"
#define ANSI_FG_GREEN   "\33[1;32m"
#define ANSI_NONE       "\33[0m"

typedef enum {
  NPC_RUNNING = 0,
  NPC_END,
  NPC_ABORT
} npc_state_t;

typedef struct {
  npc_state_t state;
  uint32_t halt_pc;
  int halt_ret;
} npc_state;

extern npc_state npc_state_info;
extern bool is_sim_end;

#endif