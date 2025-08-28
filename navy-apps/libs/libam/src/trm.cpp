#include <am.h>
#include <stdlib.h>

#define HEAP_SIZE (8 * 1024 * 1024)
static unsigned char _heap[HEAP_SIZE];
Area heap = {_heap, _heap + HEAP_SIZE};

void putch(char ch) { putchar(ch); }

void halt(int code) { exit(code); }
