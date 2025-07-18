#include "mtrace.h"

static MTrace m_trace;

void init_mtrace() {
  m_trace.current = 0;
  memset(m_trace.traces, 0, sizeof(m_trace.traces));
  Log("Memory trace initialized\n");
}

void mtrace_read(uint32_t addr, uint32_t data) {
  m_trace.traces[m_trace.current].addr = addr;
  m_trace.traces[m_trace.current].data = data;
  m_trace.traces[m_trace.current].wmask = 0;
  m_trace.traces[m_trace.current].is_write = false;
  m_trace.current = (m_trace.current + 1) % MAX_MTRACE_LEN;
}

void mtrace_write(uint32_t addr, uint32_t data, uint8_t wmask) {
  m_trace.traces[m_trace.current].addr = addr;
  m_trace.traces[m_trace.current].data = data;
  m_trace.traces[m_trace.current].wmask = wmask;
  m_trace.traces[m_trace.current].is_write = true;
  m_trace.current = (m_trace.current + 1) % MAX_MTRACE_LEN;
}

void mtrace_display() {
  Log("Recent memory trace:\n");

  for (int i = 0; i < MAX_MTRACE_LEN; i++) {
    int index = (m_trace.current + i) % MAX_MTRACE_LEN;
    MemTrace *trace = &m_trace.traces[index];

    if (trace->addr == 0)
      continue;

    if (trace->is_write) {
      Log("   WRITE 0x%08x: 0x%08x (mask: 0x%02x)\n", trace->addr, trace->data,
          trace->wmask);
    } else {
      Log("   READ  0x%08x: 0x%08x\n", trace->addr, trace->data);
    }
  }
}

void log_mem_access(uint32_t addr, uint32_t data, bool is_write, int len) {
#ifdef CONFIG_MTRACE
  if (is_write) {
    mtrace_write(addr, data, (1 << len) - 1);
  } else {
    mtrace_read(addr, data);
  }
#endif
}
