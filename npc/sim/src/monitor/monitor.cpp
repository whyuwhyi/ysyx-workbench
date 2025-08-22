#include <assert.h>
#include <common.h>
#include <defs.h>
#include <device/device.h>
#include <getopt.h>
#include <memory/memory.h>
#include <stdarg.h>
#include <stdio.h>

NPCState npc_state = {.state = NPC_STOP};
extern bool is_batch_mode;

static char *log_file = NULL;
static char *img_file = NULL;
static char *elf_file = NULL;
static char *diff_so_file = NULL;
static int difftest_port = 1234;

static int parse_args(int argc, char *argv[]) {
  const struct option table[] = {
      {"batch", no_argument, NULL, 'b'},
      {"log", required_argument, NULL, 'l'},
      {"diff", required_argument, NULL, 'd'},
      {"port", required_argument, NULL, 'p'},
      {"help", no_argument, NULL, 'h'},
      {"elf", required_argument, NULL, 'e'},
      {0, 0, NULL, 0},
  };
  int o;
  while ((o = getopt_long(argc, argv, "-bhl:d:p:e:", table, NULL)) != -1) {
    switch (o) {
    case 'b':
      is_batch_mode = true;
      break;
    case 'p':
      sscanf(optarg, "%d", &difftest_port);
      break;
    case 'l':
      log_file = optarg;
      break;
    case 'd':
      diff_so_file = optarg;
      break;
    case 'e':
      elf_file = optarg;
      break;
    case 1:
      img_file = optarg;
      return 0;
    default:
      printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
      printf("\t-b,--batch              run with batch mode\n");
      printf("\t-l,--log=FILE           output log to FILE\n");
      printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
      printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
      printf("\t-e,--elf=FILE           load ELF file FILE\n");
      printf("\n");
      exit(0);
    }
  }
  return 0;
}

void init_monitor(int argc, char *argv[]) {
  Log("Monitor initializing...");
  parse_args(argc, argv);
  init_log(log_file);

#ifdef CONFIG_ITRACE
  void init_disasm();
  init_disasm();
  init_itrace();
#endif

#ifdef CONFIG_MTRACE
  init_mtrace();
#endif

#ifdef CONFIG_FTRACE
  init_ftrace(elf_file);
#endif

  init_sdb();

  init_device();

  long img_size = load_image(img_file);

  sim_init();

#ifdef CONFIG_DIFFTEST
  init_difftest(diff_so_file, img_size, 1234);
#endif

  if (is_batch_mode) {
    Log("Running in batch mode");
  }

  Log("Monitor initialized");
}

void engine_start() {
  if (is_batch_mode) {
    npc_cpu_exec(-1);
  } else {
    Log("Starting interactive debugger...");
    sdb_mainloop();
  }

  sim_exit();
}

bool is_exit_status_bad() {
  bool good = (npc_state.state == NPC_END && npc_state.halt_ret == 0) ||
              (npc_state.state == NPC_QUIT);
  return !good;
}

void assert_fail_msg() {
  Log("Assertion failure at pc = 0x%08x", npc_state.halt_pc);
}
