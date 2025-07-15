#include "ftrace.h"

static FTrace f_trace;

void init_ftrace() {
  f_trace.current = 0;
  f_trace.call_depth = 0;
  memset(f_trace.traces, 0, sizeof(f_trace.traces));
  printf("Function trace initialized\n");
}

void ftrace_call(uint32_t caller_pc, uint32_t target_pc) {
  f_trace.traces[f_trace.current].caller_pc = caller_pc;
  f_trace.traces[f_trace.current].target_pc = target_pc;
  f_trace.traces[f_trace.current].is_call = true;
  f_trace.traces[f_trace.current].depth = f_trace.call_depth;
  snprintf(f_trace.traces[f_trace.current].func_name, MAX_FUNC_NAME, 
           "func_0x%08x", target_pc);
  
  f_trace.call_depth++;
  f_trace.current = (f_trace.current + 1) % MAX_FTRACE_LEN;
}

void ftrace_ret(uint32_t ret_pc, uint32_t target_pc) {
  if (f_trace.call_depth > 0) {
    f_trace.call_depth--;
  }
  
  f_trace.traces[f_trace.current].caller_pc = ret_pc;
  f_trace.traces[f_trace.current].target_pc = target_pc;
  f_trace.traces[f_trace.current].is_call = false;
  f_trace.traces[f_trace.current].depth = f_trace.call_depth;
  snprintf(f_trace.traces[f_trace.current].func_name, MAX_FUNC_NAME, 
           "func_0x%08x", target_pc);
  
  f_trace.current = (f_trace.current + 1) % MAX_FTRACE_LEN;
}

void ftrace_display() {
  printf("Recent function trace:\n");
  
  for (int i = 0; i < MAX_FTRACE_LEN; i++) {
    int index = (f_trace.current + i) % MAX_FTRACE_LEN;
    FuncTrace *trace = &f_trace.traces[index];
    
    if (trace->caller_pc == 0) continue;
    
    // Print indentation based on call depth
    for (int j = 0; j < trace->depth; j++) {
      printf("  ");
    }
    
    if (trace->is_call) {
      printf("call  [0x%08x] %s\n", trace->target_pc, trace->func_name);
    } else {
      printf("ret   [0x%08x] %s\n", trace->target_pc, trace->func_name);
    }
  }
}