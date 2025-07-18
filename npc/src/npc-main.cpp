#include "common.h"
#include "cpu/cpu.h"
#include "cpu/simulator.h"
#include <getopt.h>

// Function declarations
void init_monitor(int argc, char *argv[]);
void monitor_exit(void);
void engine_start();
bool is_exit_status_bad(void);

int main(int argc, char **argv) {
  /* Initialize the monitor. */
  init_monitor(argc, argv);
  
  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
