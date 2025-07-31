#include <am.h>
#include <klib-macros.h>

extern char _heap_start;
int main(const char *args);

extern char _pmem_start;
#define PMEM_SIZE (128 * 1024 * 1024)
#define PMEM_END ((uintptr_t)&_pmem_start + PMEM_SIZE)

Area heap = RANGE(&_heap_start, PMEM_END);
static const char mainargs[MAINARGS_MAX_LEN] =
    MAINARGS_PLACEHOLDER; // defined in CFLAGS

void putch(char ch) {
  // volatile char *uart_tx = (volatile char *)0xa00003f8;
  // *uart_tx = ch;
  asm volatile("sb %1, 0(%0)" : : "r"((char *)0xa00003f8), "r"(ch) : "memory");
}

void halt(int code) {

  asm volatile("mv a0, %0; ebreak" : : "r"(code));

  while (1) {
    // Infinite loop to halt the system
  }
}

void _trm_init() {
  int ret = main(mainargs);
  halt(ret);
}
