#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  uintptr_t sp = (uintptr_t)args;
  int argc = *(int *)sp;
  char *argv[argc + 1];
  for (int i = 0; i < argc; i++) {
    sp += sizeof(char *);
    argv[i] = *(char **)sp;
  }

  argv[argc] = NULL;
  sp += sizeof(char *);

  int envc = 0;
  while (sp) {
    envc++;
    sp += sizeof(char *);
  }

  char *envp[envc + 1];

  for (; envc >= 0; envc--) {
    envp[envc] = *(char **)sp;
    sp += sizeof(char *);
  }
  printf("envc %d, argc %d\n", envc, argc);

  environ = envp;
  exit(main(argc, argv, envp));
  assert(0);
}
