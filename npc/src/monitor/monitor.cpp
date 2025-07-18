#include <assert.h>
#include <common.h>
#include <cpu/cpu.h>
#include <cpu/simulator.h>
#include <memory/pmem.h>
#include <stdio.h>
#include <getopt.h>

extern void init_sdb();
extern void sdb_mainloop();
extern void sdb_set_batch_mode();
extern "C" bool is_exit_status_bad();

// Forward declaration
long load_image(char *img_file);

#ifdef CONFIG_ITRACE
extern void init_disasm();
#endif

static bool is_batch_mode = false;
static char *log_file = NULL;
static char *img_file = NULL;

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

void monitor_init(void) {
  Log("Monitor initializing...");

  // Initialize CPU and subsystems
  init_cpu();

  // Initialize SDB
  init_sdb();

  // Initialize disassembly if itrace is enabled
#ifdef CONFIG_ITRACE
  init_disasm();
#endif

  Log("Monitor initialized");
}

void init_monitor(int argc, char *argv[]) {
  /* Parse arguments. */
  parse_args(argc, argv);

  /* Open the log file. */
  init_log(log_file);

  /* Initialize monitor. */
  monitor_init();
  
  /* Initialize simulator. */
  sim_init();

  if (is_batch_mode) {
    sdb_set_batch_mode();
    Log("Running in batch mode");
  }

  /* Load the program image. */
  long img_size = load_image(img_file);
}

void monitor_exit(void) { 
  Log("Monitor exited"); 
}

static void print_trap_info() {
  uint32_t pc = get_pc_value();

  if (npc_is_stopped()) {
    printf(ANSI_FMT("npc: HIT TRAP at pc = 0x%08x", ANSI_FG_GREEN) "\n", pc);

    // Display trace information if enabled
#ifdef CONFIG_ITRACE
    extern void itrace_display();
    itrace_display();
#endif

#ifdef CONFIG_MTRACE
    extern void mtrace_display();
    mtrace_display();
#endif

#ifdef CONFIG_FTRACE
    extern void ftrace_display();
    ftrace_display();
#endif
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
  monitor_exit();
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

  // Initialize difftest after loading image
#ifdef CONFIG_DIFFTEST
  init_cpu_difftest(const_cast<char *>(CONFIG_DIFFTEST_REF_PATH), size);
#endif

  return size;
}
