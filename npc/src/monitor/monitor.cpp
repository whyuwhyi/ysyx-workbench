#include <assert.h>
#include <common.h>
#include <cpu/cpu.h>
#include <getopt.h>
#include <memory/pmem.h>
#include <stdarg.h>
#include <stdio.h>

NPCState npc_state = {.state = NPC_STOP};

static bool is_batch_mode = false;
static char *log_file = NULL;
static char *img_file = NULL;
static char *elf_file = NULL;

FILE *log_fp = NULL;
static bool log_enable_flag = true;

void init_log(const char *log_file) {
  log_fp = NULL; // Default: no log file, only console output
  if (log_file != NULL) {
    FILE *fp = fopen(log_file, "w");
    Assert(fp, "Can not open '%s'", log_file);
    log_fp = fp;
    Log("Log is written to %s", log_file);
  } else {
    Log("Log is written to stdout");
  }
}

bool log_enable() { return log_enable_flag; }

void log_set_enable(bool enable) { log_enable_flag = enable; }

static int parse_args(int argc, char *argv[]) {
  const struct option long_options[] = {{"batch", no_argument, NULL, 'b'},
                                        {"help", no_argument, NULL, 'h'},
                                        {"log", required_argument, NULL, 'l'},
                                        {"elf", required_argument, NULL, 'e'},
                                        {0, 0, NULL, 0}};

  int c;
  while ((c = getopt_long(argc, argv, "-bhl:e:", long_options, NULL)) != -1) {
    switch (c) {
    case 'b':
      is_batch_mode = true;
      break;
    case 'h':
      printf("Usage: %s [OPTION]... IMAGE\n", argv[0]);
      printf("Options:\n");
      printf("  -b, --batch         run in batch mode (non-interactive)\n");
      printf("  -l, --log=FILE      output log to FILE\n");
      printf("  -e, --elf=FILE      specify ELF file for debugging\n");
      printf("  -h, --help          display this help and exit\n");
      exit(0);
    case 'l':
      log_file = optarg;
      break;
    case 'e':
      elf_file = optarg;
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
  sim_init();

  if (is_batch_mode) {
    sdb_set_batch_mode();
    Log("Running in batch mode");
  }

  long img_size = load_image(img_file);
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

void set_npc_state(int state, vaddr_t pc, int halt_ret) {
  npc_state.state = state;
  npc_state.halt_pc = pc;
  npc_state.halt_ret = halt_ret;
}

void assert_fail_msg() {
  Log("Assertion failure at pc = 0x%08x", npc_state.halt_pc);
}
