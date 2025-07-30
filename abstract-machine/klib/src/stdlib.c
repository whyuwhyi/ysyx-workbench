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

static uintptr_t addr = 0;
void *malloc(size_t size) {
  if (addr == 0) {
    addr = (uintptr_t)heap.start;
  }
  uintptr_t start = addr;
  addr = ROUNDUP(addr, size);

  return (void *)start;
  return NULL;
}

void free(void *ptr) {}

#endif
