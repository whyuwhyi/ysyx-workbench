#ifndef __COMMON_H__
#define __COMMON_H__

#include <assert.h>
#include <generated/autoconf.h>
#include <inttypes.h>
#include <macro.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint32_t word_t;
typedef int32_t sword_t;
#define FMT_WORD "0x%08" PRIx32

typedef word_t paddr_t;
#define FMT_PADDR "0x%08" PRIx32

enum { NPC_RUNNING, NPC_STOP, NPC_END, NPC_ABORT, NPC_QUIT };

typedef struct {
  int state;
  paddr_t halt_pc;
  uint32_t halt_ret;
} NPCState;

extern NPCState npc_state;

bool log_enable();
void init_log(const char *log_file);
void log_set_enable(bool enable);

extern FILE *log_fp;

#define log_write(...)                                                         \
  do {                                                                         \
    extern bool log_enable();                                                  \
    extern FILE *log_fp;                                                       \
    if (log_enable() && log_fp != NULL) {                                      \
      fprintf(log_fp, __VA_ARGS__);                                            \
      fflush(log_fp);                                                          \
    }                                                                          \
  } while (0)

#define _Log(...)                                                              \
  do {                                                                         \
    printf(__VA_ARGS__);                                                       \
    log_write(__VA_ARGS__);                                                    \
  } while (0)

#define Log(format, ...)                                                       \
  _Log(ANSI_FMT("[%s:%d %s] " format, ANSI_FG_BLUE) "\n", __FILE__, __LINE__,  \
       __func__, ##__VA_ARGS__)

#define Log_n(format, ...)                                                     \
  _Log(ANSI_FMT("[%s:%d %s] " format, ANSI_FG_BLUE), __FILE__, __LINE__,       \
       __func__, ##__VA_ARGS__)

#define Assert(cond, format, ...)                                              \
  do {                                                                         \
    if (!(cond)) {                                                             \
      fflush(stdout);                                                          \
      fprintf(stderr, ANSI_FMT(format, ANSI_FG_RED) "\n", ##__VA_ARGS__);      \
      extern FILE *log_fp;                                                     \
      fflush(log_fp);                                                          \
      extern void assert_fail_msg();                                           \
      assert_fail_msg();                                                       \
      assert(cond);                                                            \
    }                                                                          \
  } while (0)

#define panic(format, ...) Assert(0, format, ##__VA_ARGS__)

#define TODO() panic("please implement me")

#endif
