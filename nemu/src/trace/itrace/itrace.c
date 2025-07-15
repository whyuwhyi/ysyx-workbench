#include "itrace.h"
#include <memory/vaddr.h>

static InstTrace i_trace;

void init_itrace() {
  i_trace.current = 0;
  memset(i_trace.inst_addr, 0, sizeof(i_trace.inst_addr));
  Log("Instruction trace initialized\n");
}

void itrace_push(paddr_t pc) {
  i_trace.inst_addr[i_trace.current++] = pc;
  i_trace.current %= MAX_TRACE_LEN;
  Log("itrace_push: %lu, pc = " FMT_WORD "\n", i_trace.current, pc);
}

void itrace_display() {
  Log("Recent instruction trace:\n");
  char buffer[32];
  int index = 0;
  int ilen = 4;
  vaddr_t pc = i_trace.inst_addr[0];
  uint32_t inst = 0;

  while (index < MAX_TRACE_LEN && pc != 0) {
    if (((index + 1) % MAX_TRACE_LEN) == i_trace.current) {
      Log("-->");
    } else {
      Log("   ");
    }
    inst = vaddr_ifetch(pc, ilen);
    void disassemble(char *str, int size, uint64_t pc, uint8_t *code,
                     int nbyte);
    disassemble(buffer, sizeof(buffer), pc, (uint8_t *)&inst, ilen);
    Log("" FMT_WORD ":\t%08x:\t%s\n", pc, inst, buffer);
    pc = i_trace.inst_addr[++index];
  }
}
