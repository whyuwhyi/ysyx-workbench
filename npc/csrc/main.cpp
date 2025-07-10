#include "include/common.h"
#include "include/memory.h"
#include "include/simulator.h"
#include "include/monitor.h"

int main(int argc, char **argv) {
  monitor_init();
  sim_init();
  
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <image_file>\n", argv[0]);
    return -1;
  }
  
  long size = load_image(argv[1]);

  while (!is_sim_end) {
    run(1);
  }

  sim_exit();
  monitor_exit();
  return 0;
}
