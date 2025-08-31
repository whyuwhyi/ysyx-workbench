#include <SDL.h>
#include <nterm.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() { sh_printf("sh> "); }

static void sh_handle_cmd(const char *cmd) {
  char temp_cmd[128];
  strcpy(temp_cmd, cmd);
  temp_cmd[strlen(cmd) - 1] = '\0';

  char *argv[16];
  int argc = 0;
  char *token = strtok(temp_cmd, " ");
  argv[argc++] = token;
  while (token != NULL && argc < 15) {
    token = strtok(NULL, " ");
    argv[argc++] = token;
  }

  printf("Executing command: %s\n", argv[0]);
  printf("Arguments count: %d\n", argc);
  for (int i = 0; i < argc; i++) {
    printf("argv[%d]: %s\n", i, argv[i]);
  }
  while (1)
    ;

  execvp(argv[0], argv);
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();
  setenv("PATH", "/bin", 0);

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
