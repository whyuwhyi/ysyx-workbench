#include "syscall.h"
#include <common.h>

#define SYSCALL_TRACE 1

static void strace(int id);
static void sys_exit(int code) __attribute__((noreturn));
static int sys_yield(void);

static int sys_write(int fd, void *buf, size_t count);

static int sys_sbrk(intptr_t program_break);

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
  case SYS_write:
    c->GPRx = sys_write(c->GPR2, (void *)c->GPR3, c->GPR4);
    break;

  case SYS_brk:
    c->GPRx = sys_sbrk(c->GPR2);
    break;

  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }
  strace(a[0]);
}

static void sys_exit(int code) { halt(code); }

static int sys_yield(void) {
  yield();
  return 0;
}

static int sys_write(int fd, void *buf, size_t count) {
  if (fd == 1 || fd == 2) {
    for (size_t i = 0; i < count; i++) {
      putch(((char *)buf)[i]);
    }
    return count;
  }
  return -1;
}

static int sys_sbrk(intptr_t program_break) { return 0; }

const char *syscall_name[] = {
    "SYS_exit",  "SYS_yield",  "SYS_open",   "SYS_read",   "SYS_write",
    "SYS_kill",  "SYS_getpid", "SYS_close",  "SYS_lseek",  "SYS_brk",
    "SYS_fstat", "SYS_time",   "SYS_signal", "SYS_execve", "SYS_fork",
    "SYS_link",  "SYS_unlink", "SYS_wait",   "SYS_times",  "SYS_gettimeofday"};

static void strace(int id) {
#ifdef SYSCALL_TRACE
  Log("Syscall %s is invoked.", syscall_name[id]);
#endif
}
