#include "common.h"
#include "cpu/cpu.h"
#include <getopt.h>


int main(int argc, char **argv) {
  /* Initialize the monitor. */
  init_monitor(argc, argv);
  
  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
