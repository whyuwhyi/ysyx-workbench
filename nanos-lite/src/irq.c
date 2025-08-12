#include <common.h>

void do_syscall(Context *c);
static Context *do_event(Event e, Context *c) {
  switch (e.event) {
  case EVENT_YIELD:
    Log("Yielding...");
    break;
  case EVENT_SYSCALL:
    Log("Handling syscall...");
    do_syscall(c);
  default:
    panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
