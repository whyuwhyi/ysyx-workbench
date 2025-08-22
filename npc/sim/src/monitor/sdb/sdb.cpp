#include "sdb.h"
#include <defs.h>
#include <memory/memory.h>
#include <readline/history.h>
#include <readline/readline.h>

bool is_batch_mode = false;

// Forward declarations
void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin.
 */
static char *rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(npc) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  npc_cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  npc_state.state = NPC_QUIT;
  return -1;
}

static int cmd_help(char *args);
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);
static int cmd_p(char *args);
static int cmd_w(char *args);
static int cmd_d(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler)(char *);
} cmd_table[] = {
    {"help", "Display information about all supported commands", cmd_help},
    {"c", "Continue the execution of the program", cmd_c},
    {"q", "Exit NPC", cmd_q},
    {"si", "Step an instruction exactly", cmd_si},
    {"info", "Show information about registers or watchpoints", cmd_info},
    {"x", "Scan memory according to EXPR", cmd_x},
    {"p", "Evaluate EXPR and print the result", cmd_p},
    {"w", "Set a watchpoint at EXPR", cmd_w},
    {"d", "Delete watchpoint N", cmd_d},
};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  } else {
    for (i = 0; i < NR_CMD; i++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args) {
  char *arg = args ? strtok(args, " ") : NULL;
  char *remaining = strtok(NULL, " ");
  uint64_t n = 1;

  if (arg != NULL && sscanf(arg, "%lu", &n) != 1) {
    printf("Invalid argument: %s\nUsage: si [N]\n", arg);
    return 0;
  }

  if (remaining != NULL) {
    printf("Too many arguments\nUsage: si [N]\n");
    return 0;
  }

  npc_cpu_exec(n);
  return 0;
}

static int cmd_info(char *args) {
  if (args == NULL) {
    printf("Too few argument\nUsage: info SUBCMD\n");
    return 0;
  }

  char *arg = strtok(args, " ");
  char *remaining = strtok(NULL, " ");

  if (arg == NULL) {
    printf("Too few argument\nUsage: info SUBCMD\n");
    return 0;
  }

  if (remaining != NULL) {
    printf("Too many arguments\nUsage: info SUBCMD\n");
    return 0;
  }

  if (strcmp(arg, "r") == 0) {
    npc_reg_display();
  } else if (strcmp(arg, "w") == 0) {
    display_wp();
  } else {
    printf("Unknown subcommand '%s'\n", arg);
  }

  return 0;
}

static int cmd_x(char *args) {
  if (args == NULL) {
    printf("Usage: x N EXPR\n");
    return 0;
  }

  char *arg_end = args + strlen(args);
  char *arg1 = strtok(args, " ");
  char *temptr = arg1 ? arg1 + strlen(arg1) + 1 : NULL;
  char *exp = (temptr && temptr < arg_end) ? temptr : NULL;
  int n = -1;

  while (exp != NULL && *exp == ' ') {
    exp++;
  }

  if (arg1 == NULL || exp == NULL || *exp == '\0') {
    printf("Too few argument\nUsage: x N EXPR\n");
    return 0;
  }

  if (sscanf(arg1, "%d", &n) != 1 || n <= 0) {
    printf("Invalid argument: '%s'\nUsage: x N EXPR\n", arg1);
    return 0;
  }

  bool success = true;
  word_t addr = expr(exp, &success);
  word_t value = 0;

  if (success == false) {
    printf("Invalid expression\n");
    return 0;
  }

  printf("address \t hex \t\t dec\n");
  for (int i = 0; i < n; ++i) {
    value = paddr_read(addr, 4);
    printf("0x%08x\t 0x%08x\t %d\n", addr, value, value);
    addr += 4;
  }
  printf("\n");

  return 0;
}

static int cmd_p(char *args) {
  while (args != NULL && *args == ' ') {
    args++;
  }

  if (args == NULL || *args == '\0') {
    printf("Too few arguments\nUsage: p EXPR\n");
    return 0;
  }

  bool success = true;
  word_t result = expr(args, &success);

  if (success == false) {
    printf("Invalid expression\n");
    return 0;
  }

  printf("result = %d (0x%x)\n", (int)result, (int)result);
  return 0;
}

static int cmd_w(char *args) {
  while (args != NULL && *args == ' ') {
    args++;
  }

  if (args == NULL || *args == '\0') {
    printf("Too few arguments\nUsage: w EXPR\n");
    return 0;
  }

  bool success = true;
  word_t value = expr(args, &success);

  if (success == false) {
    printf("Invalid expression\n");
    return 0;
  }

  new_wp(args, value);
  printf("Watchpoint set on expression: %s\n", args);

  return 0;
}

static int cmd_d(char *args) {
  char *arg = args ? strtok(args, " ") : NULL;
  char *remaining = strtok(NULL, " ");

  if (arg == NULL) {
    printf("Too few arguments\nUsage: d N\n");
    return 0;
  }

  if (remaining != NULL) {
    printf("Too many arguments\nUsage: d N\n");
    return 0;
  }

  int n;
  if (sscanf(arg, "%d", &n) != 1) {
    printf("Invalid argument: %s\n", arg);
    return 0;
  }

  free_wp(n);
  return 0;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL;) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) {
      continue;
    }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

    int i;
    for (i = 0; i < NR_CMD; i++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) {
          return;
        }
        break;
      }
    }

    if (i == NR_CMD) {
      printf("Unknown command '%s'\n", cmd);
    }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
