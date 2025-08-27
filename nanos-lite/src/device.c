#include "fs.h"
#include <common.h>
#include <stdint.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
#define MULTIPROGRAM_YIELD() yield()
#else
#define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) [AM_KEY_##key] = #key,

static const char *keyname[256]
    __attribute__((used)) = {[AM_KEY_NONE] = "NONE", AM_KEYS(NAME)};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  for (size_t i = 0; i < len; i++) {
    putch(((char *)buf)[i]);
  }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T key = io_read(AM_INPUT_KEYBRD);
  if (key.keycode == AM_KEY_NONE) {
    return 0;
  } else {
    return snprintf(buf, len, "%s %s(%d)\n", key.keydown ? "kd" : "ku",
                    keyname[key.keycode], key.keycode);
  }
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  return snprintf(buf, len, "WIDTH:%d\nHEIGHT:%d\n", cfg.width, cfg.height);
}

int fs_open(const char *pathname, int flags, int mode);

size_t fb_write(const void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  int width = cfg.width;
  int pixel_start = offset / 4;
  int x = pixel_start % width;
  int y = pixel_start / width;
  int w = len / 4;
  io_write(AM_GPU_FBDRAW, x, y, (void *)buf, w, 1, true);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
