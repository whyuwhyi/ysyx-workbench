#include "common.h"
#include "cpu/cpu.h"
#include "cpu/simulator.h"
#include <getopt.h>

// Function declarations
void monitor_init(void);
void monitor_exit(void);
long load_image(char *img_file);
bool is_exit_status_bad(void);
void set_batch_mode();
void start_monitor();

// External variables
extern bool is_sim_end;

static void print_usage(const char *prog_name) {
  printf("Usage: %s [OPTION]... IMAGE\n", prog_name);
  printf("Options:\n");
  printf("  -b, --batch    run in batch mode (non-interactive)\n");
  printf("  -h, --help     display this help and exit\n");
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

int main(int argc, char **argv) {
  bool batch_mode = false;
  char *img_file = NULL;

  // Parse command line options
  static struct option long_options[] = {{"batch", no_argument, 0, 'b'},
                                         {"help", no_argument, 0, 'h'},
                                         {0, 0, 0, 0}};

  int c;
  while ((c = getopt_long(argc, argv, "bh", long_options, NULL)) != -1) {
    switch (c) {
    case 'b':
      batch_mode = true;
      break;
    case 'h':
      print_usage(argv[0]);
      return 0;
    case '?':
      print_usage(argv[0]);
      return 1;
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "Error: No image file specified\n");
    print_usage(argv[0]);
    return 1;
  }

  img_file = argv[optind];

  // Initialize systems
  monitor_init();
  sim_init();

  if (batch_mode) {
    set_batch_mode();
    Log("Running in batch mode");
  }

  // Load the program image
  long size = load_image(img_file);

  if (batch_mode) {
    npc_cpu_exec(-1);
  } else {
    // Start interactive debugger
    start_monitor();
  }

  print_trap_info();

  sim_exit();
  monitor_exit();

  return npc_is_stopped() ? 0 : 1;
}
