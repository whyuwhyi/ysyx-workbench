#include <am.h>
#include <stdlib.h>
#include <unistd.h>

#define HEAP_SIZE (8 * 1024 * 1024)
static unsigned char _heap[HEAP_SIZE];
Area heap = {_heap, _heap + HEAP_SIZE};

void putch(char ch) {}

void halt(int code) {}
