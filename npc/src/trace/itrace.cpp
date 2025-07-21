#include <common.h>

#define MAX_IRINGBUF 24

typedef struct {
  uint32_t pc;
  uint32_t inst;
  char disasm[64];
} TraceEntry;

typedef struct {
  TraceEntry entries[MAX_IRINGBUF];
  size_t current;
} InstTrace;

static InstTrace itrace;

void init_itrace() {
  itrace.current = 0;
  memset(itrace.entries, 0, sizeof(itrace.entries));
  Log("Instruction trace initialized");
}

void itrace_push(uint32_t pc, uint32_t inst) {
  TraceEntry *entry = &itrace.entries[itrace.current];
  entry->pc = pc;
  entry->inst = inst;
  
  // Generate disassembly
  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  disassemble(entry->disasm, sizeof(entry->disasm), pc, (uint8_t *)&inst, 4);
  
  itrace.current = (itrace.current + 1) % MAX_IRINGBUF;
}

void itrace_display() {
  Log("Recent instruction trace:");
  
  for (int i = 0; i < MAX_IRINGBUF; i++) {
    int index = (itrace.current + i) % MAX_IRINGBUF;
    TraceEntry *entry = &itrace.entries[index];
    
    if (entry->pc == 0) continue;  // Skip empty entries
    
    // Mark the most recently executed instruction
    if (index == ((itrace.current - 1 + MAX_IRINGBUF) % MAX_IRINGBUF)) {
      Log("-->");
    } else {
      Log("   ");
    }
    
    Log("0x%08x:\t%08x:\t%s", entry->pc, entry->inst, entry->disasm);
  }
}