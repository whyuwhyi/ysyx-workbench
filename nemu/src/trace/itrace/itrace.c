#include "itrace.h"
#include <memory/vaddr.h>

static InstTrace i_trace;

void init_itrace() {
  i_trace.current = 0;
  memset(i_trace.inst_addr, 0, sizeof(i_trace.inst_addr));
}

void itrace_push(paddr_t pc) {
  i_trace.inst_addr[i_trace.current] = pc;
  i_trace.current %= MAX_TRACE_LEN;
  i_trace.current++;
}

void itrace_display() {
  char buffer[128];
  int index = 0;
  int ilen = 4;
  vaddr_t pc = i_trace.inst_addr[0];
  uint32_t inst = 0;
  while (index < MAX_TRACE_LEN && pc != 0) {
    inst = vaddr_ifetch(pc, ilen);
    void disassemble(char *str, int size, uint64_t pc, uint8_t *code,
                     int nbyte);
    disassemble(buffer, sizeof(buffer), pc, (uint8_t *)&inst, ilen);
    printf("%08x:\t%08x:\t%s\n", pc, inst, buffer);
    pc = i_trace.inst_addr[++index];
  }
}
