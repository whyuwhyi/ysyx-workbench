#include <am.h>
#include <common.h>
#include <memory.h>
#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() { current = &pcb_boot; }

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    // Log("Hello World from Nanos-lite with arg '%p' for the %dth time!",
    // (uintptr_t)arg, j);
    j++;
    yield();
  }
}

void init_proc() {
  context_kload(&pcb[0], hello_fun, "1");
  char *args[] = {"/bin/nterm", NULL};
  context_uload(&pcb[1], "/bin/nterm", args, NULL);
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

void context_uload(PCB *pcb, const char *filename, char *const argv[],
                   char *const envp[]) {
  uintptr_t sp = (uintptr_t)new_page(8) + 8 * PGSIZE;
  char *args[16];
  char *envs[16];
  int envc = 0;
  int argc = 0;

  if (envp) {
    for (; envp[envc]; envc++) {
      sp -= strlen(envp[envc]) + 1;
      strcpy((char *)sp, envp[envc]);
      envs[envc] = (char *)sp;
    }
  }

  if (argv) {
    for (; argv[argc]; argc++) {
      sp -= strlen(argv[argc]) + 1;
      strcpy((char *)sp, argv[argc]);
      args[argc] = (char *)sp;
    }
  }

  sp -= sizeof(char *);
  *(int *)sp = 0;
  for (; envc; envc--) {
    sp -= sizeof(char **);
    *(char **)sp = envs[envc - 1];
  }

  sp -= sizeof(char *);
  *(int *)sp = 0;
  for (int i = argc; i; i--) {
    sp -= sizeof(char **);
    *(char **)sp = args[i - 1];
  }

  sp -= sizeof(int *);
  *(int *)sp = argc;

  uintptr_t entry = loader(pcb, filename);
  Area stack_area = (Area){pcb->stack, pcb->stack + STACK_SIZE};
  pcb->cp = ucontext(NULL, stack_area, (void *)entry);
  pcb->cp->GPRx = sp;
}
