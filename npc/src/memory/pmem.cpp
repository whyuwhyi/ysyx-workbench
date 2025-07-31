#include <assert.h>
#include <cpu/cpu.h>
#include <device/device.h>
#include <memory/pmem.h>
#include <sys/types.h>

static uint8_t pmem[CONFIG_MSIZE];

static inline bool in_pmem(uint32_t addr) {
  return addr >= CONFIG_MBASE && addr < CONFIG_MBASE + CONFIG_MSIZE;
}

uint32_t paddr_read(uint32_t addr, int len) {
  if (!in_pmem(addr)) {
    return 0;
  }

  uint32_t offset = addr - CONFIG_MBASE;
  uint32_t data = 0;

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
  mtrace_read(addr, len);
#endif

  return data;
}

void paddr_write(uint32_t addr, int len, uint32_t data) {
  if (!in_pmem(addr)) {
    return;
  }

#ifdef CONFIG_MTRACE
  mtrace_write(addr, len, data);
#endif

  uint32_t offset = addr - CONFIG_MBASE;

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

void init_mem() {
  Log("Initialize memory: base=0x%08x, size=0x%08x", CONFIG_MBASE,
      CONFIG_MSIZE);
}

extern "C" int pmem_read(int raddr) {
  if (raddr == TIMER_ADDR) {
    return timer_get_timer();
  }
  return paddr_read(raddr, 4);
}

extern "C" void pmem_write(int waddr, int wdata, char wmask) {
  if (waddr == (int)SERIAL_ADDR) {
    Log("Serial write: 0x%08x, data: 0x%08x, mask: 0x%02x", waddr, wdata,
        wmask);
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
    break;
  }
}

uint8_t *get_pmem() { return pmem; }
