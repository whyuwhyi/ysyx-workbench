#include <common.h>
#include <memory/vaddr.h>

#define MAX_TRACE_LEN 24

typedef struct {
  vaddr_t pc;
  uint32_t inst;
  char disasm[64];
} TraceEntry;

typedef struct {
  TraceEntry entries[MAX_TRACE_LEN];
  size_t current;
} InstTrace;

static InstTrace i_trace;

void init_itrace() {
  i_trace.current = 0;
  memset(i_trace.entries, 0, sizeof(i_trace.entries));
  Log("Instruction trace initialized");
}

void itrace_push(paddr_t pc, uint32_t inst, const char *disasm) {
  TraceEntry *entry = &i_trace.entries[i_trace.current];
  entry->pc = pc;
  entry->inst = inst;
  strncpy(entry->disasm, disasm ? disasm : "", sizeof(entry->disasm) - 1);
  entry->disasm[sizeof(entry->disasm) - 1] = '\0';
  i_trace.current = (i_trace.current + 1) % MAX_TRACE_LEN;
}

void itrace_display() {
  Log("Recent instruction trace:");
  
  for (int i = 0; i < MAX_TRACE_LEN; i++) {
    int index = (i_trace.current + i) % MAX_TRACE_LEN;
    TraceEntry *entry = &i_trace.entries[index];
    
    if (entry->pc == 0) continue;  // Skip empty entries
    
    // Mark the most recently executed instruction
    if (index == ((i_trace.current - 1 + MAX_TRACE_LEN) % MAX_TRACE_LEN)) {
      Log("-->");
    } else {
      Log("   ");
    }
    
    Log("" FMT_WORD ":\t%08x:\t%s", entry->pc, entry->inst, entry->disasm);
  }
}
