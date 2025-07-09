#include "memory/pmem.h"
#include "verilator/api.h"
#include <cassert>
#include <cstdio>

#define NR_INST 100

bool is_sim_end = false;

int main(int argc, char **argv) {
  sim_init();
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <image_file>\n", argv[0]);
    return -1;
  }
  long size = load_image(argv[1]);

  while (!is_sim_end) {
    single_cycle();
    printf("........\n");
  }

  sim_exit();
  return 0;
}
