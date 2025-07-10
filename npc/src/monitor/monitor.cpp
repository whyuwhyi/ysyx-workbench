#include "memory.h"
#include <assert.h>
#include <stdio.h>

void monitor_init(void) { printf("Monitor initialized\n"); }

void monitor_exit(void) { printf("Monitor exited\n"); }

long load_image(char *img_file) {
  assert(img_file != NULL && "Image file path must not be NULL");

  FILE *fp = fopen(img_file, "rb");
  assert(fp != NULL && "Can not open image file");

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  fseek(fp, 0, SEEK_SET);
  uint8_t *pmem = get_pmem();
  int ret = fread(pmem, size, 1, fp);
  assert(ret == 1);

  fclose(fp);
  printf("Image loaded: %s, size = %ld bytes\n", img_file, size);
  return size;
}
