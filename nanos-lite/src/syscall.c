#include "syscall.h"
#include <common.h>

static void sys_exit(int code) __attribute__((noreturn));
static int sys_yield(void);

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
  case SYS_exit:
    sys_exit(c->GPR2);
    break;
  case SYS_yield:
    c->GPRx = sys_yield();
    break;
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }
}

static void sys_exit(int code) { halt(code); }

static int sys_yield(void) {
  yield();
  return 0;
}
