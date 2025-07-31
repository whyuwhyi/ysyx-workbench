#include <am.h>
#include <nemu.h>

#define WIDTH_ADDR VGACTL_ADDR
#define HEIGHT_ADDR (VGACTL_ADDR + 2)
#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  int i;
  int w = inw(WIDTH_ADDR);
  int h = inw(HEIGHT_ADDR);
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i++)
    fb[i] = i;
  outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T){.present = true,
                           .has_accel = false,
                           .width = inw(WIDTH_ADDR),
                           .height = inw(HEIGHT_ADDR),
                           .vmemsz = 0};
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int x = ctl->x;
  int y = ctl->y;
  int w = ctl->w;
  int h = ctl->h;
  int width = inw(WIDTH_ADDR);
  int height = inw(HEIGHT_ADDR);
  uint32_t *pixels = ctl->pixels;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;

  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      if (x + i >= 0 && x + i < width && y + j >= 0 && y + j < height) {
        fb[(y + j) * width + (x + i)] = pixels[j * w + i];
      }
    }
  }

  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) { status->ready = true; }
