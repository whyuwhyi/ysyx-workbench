#include <defs.h>
#include <device/device.h>
#include <memory/memory.h>
#include <sys/types.h>

uint8_t pmem[CONFIG_MSIZE];

IFDEF(CONFIG_DIFFTEST, extern bool ref_skip_difftest);

static inline bool in_pmem(paddr_t addr) {
  return addr >= CONFIG_MBASE && addr < CONFIG_MBASE + CONFIG_MSIZE;
}

word_t paddr_read(paddr_t addr, int len) {
  if (!in_pmem(addr)) {
    Log("Read from invalid address: 0x%08x", addr);
    return 0;
  }

  paddr_t offset = addr - CONFIG_MBASE;
  word_t data = 0;

  switch (len) {
  case 1:
    data = *(uint8_t *)(pmem + offset);
    break;
  case 2:
    data = *(uint16_t *)(pmem + offset);
    break;
  case 4:
    data = *(uint32_t *)(pmem + offset);
    break;
  default:
    assert(0);
  }

#ifdef CONFIG_MTRACE
  mtrace_read(addr, len, data);
#endif

  return data;
}

void paddr_write(paddr_t addr, int len, word_t data) {
  if (!in_pmem(addr)) {
    Log("Write to invalid address: 0x%08x", addr);
    return;
  }

#ifdef CONFIG_MTRACE
  mtrace_write(addr, len, data);
#endif

  paddr_t offset = addr - CONFIG_MBASE;

  switch (len) {
  case 1:
    *(uint8_t *)(pmem + offset) = data;
    break;
  case 2:
    *(uint16_t *)(pmem + offset) = data;
    break;
  case 4:
    *(uint32_t *)(pmem + offset) = data;
    break;
  default:
    assert(0);
  }
}

long load_img(const char *img_file) {
  if (img_file == NULL) {
    Log("No image file specified");
    return 0;
  }

  FILE *fp = fopen(img_file, "rb");
  if (fp == NULL) {
    Log("Cannot open image file: %s", img_file);
    return 0;
  }

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  if (size > CONFIG_MSIZE) {
    Log("Image file too large: %ld > %d", size, CONFIG_MSIZE);
    fclose(fp);
    return 0;
  }

  int ret = fread(pmem, size, 1, fp);
  if (ret != 1) {
    Log("Failed to read image file");
    fclose(fp);
    return 0;
  }

  fclose(fp);
  Log("Loaded image file: %s, size: %ld bytes", img_file, size);
  return size;
}

extern "C" int pmem_read(int raddr) {
  if (raddr == TIMER_ADDR || raddr == SERIAL_ADDR) {

    IFDEF(CONFIG_DIFFTEST, ref_skip_difftest = true);
    return timer_get_time();
  }
  return paddr_read(raddr, 4);
}

extern "C" void pmem_write(int waddr, int wdata, char wmask) {
  if (waddr == SERIAL_ADDR) {

    IFDEF(CONFIG_DIFFTEST, ref_skip_difftest = true);
    serial_putchar(wdata & 0xff);
    return;
  }

  switch (wmask) {
  case 0x1:
    paddr_write(waddr, 1, wdata);
    break;
  case 0x3:
    paddr_write(waddr, 2, wdata);
    break;
  case 0xf:
    paddr_write(waddr, 4, wdata);
    break;
  default:
    assert(0);
  }
}

long load_image(char *img_file) {
  Assert(img_file != NULL, "Image file path must not be NULL");

  FILE *fp = fopen(img_file, "rb");
  Assert(fp, "Can not open '%s'", img_file);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  fseek(fp, 0, SEEK_SET);
  int ret = fread(pmem, size, 1, fp);
  assert(ret == 1);

  fclose(fp);
  Log("Image loaded: %s, size = %ld bytes", img_file, size);

  return size;
}
