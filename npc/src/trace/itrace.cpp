#include <common.h>

#define MAX_IRINGBUF 24

typedef struct {
  uint32_t inst_addr[MAX_IRINGBUF];
  size_t current;
} InstTrace;

static InstTrace itrace;

void init_itrace() {
  itrace.current = 0;
  memset(itrace.inst_addr, 0, sizeof(itrace.inst_addr));
  Log("Instruction trace initialized");
}

void itrace_push(uint32_t pc) {
  itrace.inst_addr[itrace.current] = pc;
  itrace.current = (itrace.current + 1) % MAX_IRINGBUF;
}

void itrace_display() {
  Log("Recent instruction trace:");
  for (int i = 0; i < MAX_IRINGBUF; i++) {
    int index = (itrace.current + i) % MAX_IRINGBUF;
    if (itrace.inst_addr[index] == 0) continue;
    printf("  0x%08x\n", itrace.inst_addr[index]);
  }
}