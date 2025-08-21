#include <defs.h>

int main(int argc, char **argv) {
  sim_init();
  init_monitor(argc, argv);
  engine_start();
  return is_exit_status_bad();
}
