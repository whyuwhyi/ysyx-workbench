#include "syscall.h"
#include <common.h>
#include <fs.h>
#include <proc.h>
#include <sys/time.h>

#define SYSCALL_TRACE false
#ifndef SYSCALL_TRACE
#define SYSCALL_TRACE true
#endif

#define STRACE(...)                                                            \
  do {                                                                         \
    if (SYSCALL_TRACE)                                                         \
      Log(__VA_ARGS__);                                                        \
  } while (0)

static void sys_exit(int code) __attribute__((noreturn));
static int sys_yield(void);
static int sys_open(const char *path, int flags, int mode);
static int sys_read(int fd, void *buf, size_t count);
static int sys_write(int fd, void *buf, size_t count);
static int sys_close(int fd);
static int sys_lseek(int fd, size_t offset, int whence);

static int sys_brk(intptr_t program_break);

static int sys_execve(const char *fname, char *const argv[], char *const envp[])
    __attribute__((noreturn));

static int sys_gettimeofday(struct timeval *tv, struct timezone *tz);

void do_syscall(Context *c) {
  switch (c->GPR1) {
  case SYS_exit:
    sys_exit(c->GPR2);
    break;

  case SYS_yield:
    c->GPRx = sys_yield();
    break;
  case SYS_open:
    c->GPRx = sys_open((const char *)c->GPR2, c->GPR3, c->GPR4);
    break;
  case SYS_read:
    c->GPRx = sys_read(c->GPR2, (void *)c->GPR3, c->GPR4);
    break;
  case SYS_write:
    c->GPRx = sys_write(c->GPR2, (void *)c->GPR3, c->GPR4);
    break;
  case SYS_close:
    c->GPRx = sys_close(c->GPR2);
    break;
  case SYS_lseek:
    c->GPRx = sys_lseek(c->GPR2, c->GPR3, c->GPR4);
    break;

  case SYS_brk:
    c->GPRx = sys_brk(c->GPR2);
    break;
  case SYS_execve:
    c->GPRx = sys_execve((const char *)c->GPR2, (char *const *)c->GPR3,
                         (char *const *)c->GPR4);
    break;
  case SYS_gettimeofday:
    c->GPRx =
        sys_gettimeofday((struct timeval *)c->GPR2, (struct timezone *)c->GPR3);
    break;

  default:
    panic("Unhandled syscall ID = %d", c->GPR1);
  }
}

static void sys_exit(int code) {
  STRACE("sys_exit(%d) called", code);
  sys_execve("/bin/nterm", NULL, NULL);
}

static int sys_yield(void) {
  STRACE("sys_yield() called");
  yield();
  return 0;
}

static int sys_open(const char *path, int flags, int mode) {
  STRACE("sys_open(\"%s\", %d, %d) called", path, flags, mode);
  return fs_open(path, flags, mode);
}
static int sys_read(int fd, void *buf, size_t count) {
  STRACE("sys_read(%d, %p, %u) called", fd, buf, count);
  return fs_read(fd, buf, count);
}

static int sys_write(int fd, void *buf, size_t count) {
  STRACE("sys_write(%d, %p, %u) called", fd, buf, count);
  return fs_write(fd, buf, count);
}

static int sys_close(int fd) {
  STRACE("sys_close(%d) called", fd);
  return fs_close(fd);
}

static int sys_lseek(int fd, size_t offset, int whence) {
  STRACE("sys_lseek(%d, %u, %d) called", fd, offset, whence);
  return fs_lseek(fd, offset, whence);
}

static int sys_brk(intptr_t program_break) {
  STRACE("sys_brk(%d) called", program_break);
  return 0;
}

static int sys_execve(const char *fname, char *const argv[],
                      char *const envp[]) {
  STRACE("sys_execve(\"%s\") called", fname);
  extern PCB *current;
  context_uload(current, fname, argv, envp);
  switch_boot_pcb();
  yield();

  while (1)
    ;
}

static int sys_gettimeofday(struct timeval *tv, struct timezone *tz) {
  STRACE("sys_gettimeofday called");
  assert(tv != NULL);
  long usec = io_read(AM_TIMER_UPTIME).us;
  tv->tv_sec = usec / 1000000;
  tv->tv_usec = usec % 1000000;
  return 0;
}
