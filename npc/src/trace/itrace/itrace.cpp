#include "itrace.h"
#include <memory/pmem.h>

#ifdef CONFIG_ITRACE
extern void disassemble(char *str, int size, uint64_t pc, uint8_t *code,
                        int nbyte);
#endif

static InstTrace i_trace;

void init_itrace() {
  i_trace.current = 0;
  memset(i_trace.inst_addr, 0, sizeof(i_trace.inst_addr));
  memset(i_trace.inst_data, 0, sizeof(i_trace.inst_data));
  Log("Instruction trace initialized\n");
}

void itrace_push(uint32_t pc, uint32_t inst) {
  i_trace.inst_addr[i_trace.current] = pc;
  i_trace.inst_data[i_trace.current] = inst;
  i_trace.current = (i_trace.current + 1) % MAX_TRACE_LEN;
}

void itrace_display() {
  Log("Recent instruction trace:\n");

  for (int i = 0; i < MAX_TRACE_LEN; i++) {
    int index = (i_trace.current + i) % MAX_TRACE_LEN;
    uint32_t pc = i_trace.inst_addr[index];
    uint32_t inst = i_trace.inst_data[index];

    if (pc == 0)
      continue;

    if (index == (i_trace.current - 1 + MAX_TRACE_LEN) % MAX_TRACE_LEN) {
      Log("-->");
    } else {
      Log("   ");
    }

#ifdef CONFIG_ITRACE
    char disasm_str[128];
    disassemble(disasm_str, sizeof(disasm_str), pc, (uint8_t *)&inst, 4);
    Log(" 0x%08x: 0x%08x  %s\n", pc, inst, disasm_str);
#else
    Log(" 0x%08x: 0x%08x\n", pc, inst);
#endif
  }
}

void log_inst(uint32_t pc, uint32_t inst) {
#ifdef CONFIG_ITRACE
#ifndef ITRACE_COND
#define ITRACE_COND true
#endif
  if (ITRACE_COND) {
    // Store in trace buffer
    itrace_push(pc, inst);
    
    // Output instruction decoding in NEMU style
    printf("0x%08x: ", pc);
    
    // Print instruction bytes (little-endian for RISC-V)
    uint8_t *inst_bytes = (uint8_t *)&inst;
    for (int i = 0; i < 4; i++) {
      printf("%02x ", inst_bytes[i]);
    }
    
    // Disassemble and print instruction
    char disasm_str[128];
    disassemble(disasm_str, sizeof(disasm_str), pc, (uint8_t *)&inst, 4);
    printf("%s\n", disasm_str);
  }
#endif
}
