#include <am.h>
#include <klib-macros.h>
#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;

int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next / 65536) % 32768;
}

void srand(unsigned int seed) { next = seed; }

int abs(int x) { return (x < 0 ? -x : x); }

int atoi(const char *nptr) {
  int x = 0;
  while (*nptr == ' ') {
    nptr++;
  }
  while (*nptr >= '0' && *nptr <= '9') {
    x = x * 10 + *nptr - '0';
    nptr++;
  }
  return x;
}

#define MALLOC_ALIGN 4

static uintptr_t brk_addr = 0;

void *malloc(size_t size) {
  if (brk_addr == 0) {
    brk_addr = (uintptr_t)ROUNDUP(heap.start, MALLOC_ALIGN);
  }

  size = (size_t)ROUNDUP(size, MALLOC_ALIGN);

  if (brk_addr + size > (uintptr_t)heap.end) {
    return NULL;
  }

  uintptr_t old_brk = brk_addr;

  brk_addr += size;

  return (void *)old_brk;
}

void free(void *ptr) {}

#endif
