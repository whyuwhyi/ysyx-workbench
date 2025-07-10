#include "include/memory.h"
#include <sys/types.h>

static uint8_t pmem[MEM_SIZE];

extern "C" int pmem_read(int raddr) {
  return *(int *)(pmem + (raddr & ~0x3u) - MEM_BASE);
}

extern "C" void pmem_write(int waddr, int wdata, char wmask) {
  switch (wmask) {
  case 0x1:
    *(uint8_t *)(pmem + (waddr - MEM_BASE)) = wdata & 0xff;
    break;
  case 0x3:
    *(uint16_t *)(pmem + (waddr - MEM_BASE)) = wdata & 0xffff;
    break;
  case 0xf:
    *(uint32_t *)(pmem + (waddr - MEM_BASE)) = wdata;
    break;
  default:
    assert(0 && "Invalid write mask");
  }
}

long load_image(char *img_file) {
  assert(img_file != NULL && "Image file path must not be NULL");

  FILE *fp = fopen(img_file, "rb");
  assert(fp != NULL && "Can not open image file");

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  fseek(fp, 0, SEEK_SET);
  int ret = fread(pmem, size, 1, fp);
  assert(ret == 1);

  fclose(fp);
  return size;
}
