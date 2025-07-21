#include <assert.h>
#include <common.h>
#include <cpu/cpu.h>
#include <memory/pmem.h>
#include <stdio.h>
#include <getopt.h>
#include <stdarg.h>

NPCState npc_state = { .state = NPC_STOP };

static bool is_batch_mode = false;
static char *log_file = NULL;
static char *img_file = NULL;

FILE* log_fp = NULL;
static bool log_enable_flag = true;

void init_log(const char *log_file) {
  log_fp = NULL;
  if (log_file != NULL) {
    FILE *fp = fopen(log_file, "w");
    Assert(fp, "Can not open '%s'", log_file);
    log_fp = fp;
    Log("Log is written to %s", log_file);
  }
}

bool log_enable() {
  return log_enable_flag;
}

void log_set_enable(bool enable) {
  log_enable_flag = enable;
}


static int parse_args(int argc, char *argv[]) {
  static struct option long_options[] = {
    {"batch", no_argument, 0, 'b'},
    {"help", no_argument, 0, 'h'},
    {"log", required_argument, 0, 'l'},
    {0, 0, 0, 0}
  };

  int c;
  while ((c = getopt_long(argc, argv, "bhl:", long_options, NULL)) != -1) {
    switch (c) {
      case 'b':
        is_batch_mode = true;
        break;
      case 'h':
        printf("Usage: %s [OPTION]... IMAGE\n", argv[0]);
        printf("Options:\n");
        printf("  -b, --batch         run in batch mode (non-interactive)\n");
        printf("  -l, --log=FILE      output log to FILE\n");
        printf("  -h, --help          display this help and exit\n");
        exit(0);
      case 'l':
        log_file = optarg;
        break;
      case '?':
        printf("Usage: %s [OPTION]... IMAGE\n", argv[0]);
        exit(1);
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "Error: No image file specified\n");
    printf("Usage: %s [OPTION]... IMAGE\n", argv[0]);
    exit(1);
  }

  img_file = argv[optind];
  return 0;
}

long load_image(char *img_file) {
  Assert(img_file != NULL, "Image file path must not be NULL");

  FILE *fp = fopen(img_file, "rb");
  Assert(fp, "Can not open '%s'", img_file);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  fseek(fp, 0, SEEK_SET);
  uint8_t *pmem = get_pmem();
  int ret = fread(pmem, size, 1, fp);
  assert(ret == 1);

  fclose(fp);
  Log("Image loaded: %s, size = %ld bytes", img_file, size);

#ifdef CONFIG_DIFFTEST
  init_cpu_difftest(const_cast<char *>(CONFIG_DIFFTEST_REF_PATH), size);
#endif

  return size;
}

void init_monitor(int argc, char *argv[]) {
  parse_args(argc, argv);
  init_log(log_file);
  
  Log("Monitor initializing...");

  init_cpu();

#ifdef CONFIG_ITRACE
  init_itrace();
#endif

#ifdef CONFIG_MTRACE
  init_mtrace();
#endif

#ifdef CONFIG_FTRACE
  init_ftrace(img_file);
#endif

  init_sdb();
  sim_init();

  if (is_batch_mode) {
    sdb_set_batch_mode();
    Log("Running in batch mode");
  }

  long img_size = load_image(img_file);
  Log("Monitor initialized");
}

static void print_trap_info() {
  switch (npc_state.state) {
    case NPC_END:
    case NPC_ABORT:
      Log("npc: %s at pc = 0x%08x",
          (npc_state.state == NPC_ABORT
               ? ANSI_FMT("ABORT", ANSI_FG_RED)
               : (npc_state.halt_ret == 0
                      ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN)
                      : ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
          npc_state.halt_pc);
#ifdef CONFIG_ITRACE
      if (npc_state.halt_ret != 0) {
        itrace_display();
      }
#endif
      break;
    case NPC_QUIT:
      break;
  }
}

void engine_start() {
  if (is_batch_mode) {
    npc_cpu_exec(-1);
  } else {
    Log("Starting interactive debugger...");
    sdb_mainloop();
  }

  print_trap_info();
  sim_exit();
}

bool is_exit_status_bad() {
  bool good = (npc_state.state == NPC_END && npc_state.halt_ret == 0) ||
    (npc_state.state == NPC_QUIT);
  return !good;
}

void set_npc_state(int state, vaddr_t pc, int halt_ret) {
  npc_state.state = state;
  npc_state.halt_pc = pc;
  npc_state.halt_ret = halt_ret;
}

void assert_fail_msg() {
  Log("Assertion failure at pc = 0x%08x", npc_state.halt_pc);
}