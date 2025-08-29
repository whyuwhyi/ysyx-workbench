#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() { current = &pcb_boot; }

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!",
        (uintptr_t)arg, j);
    j++;
    yield();
  }
}

void init_proc() {
  context_kload(&pcb[0], hello_fun, "1");
  context_kload(&pcb[1], hello_fun, "1");
  switch_boot_pcb();
}

Context *schedule(Context *prev) {
  current->cp = prev;
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  return current->cp;
}

void context_kload(PCB *pcb, void *entry, void *arg) {
  Area stack_area = (Area){pcb->stack, pcb->stack + STACK_SIZE};
  pcb->cp = kcontext(stack_area, entry, arg);
}
