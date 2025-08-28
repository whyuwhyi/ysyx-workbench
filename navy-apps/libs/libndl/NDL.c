#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
// static int canvas_w = 0, canvas_h = 0;

uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv, NULL);

  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len) {
  int fd = open("/dev/events", 0);
  int real_len = read(fd, buf, len);
  close(fd);
  return real_len;
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    printf("hello\n");
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w;
    screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0)
        continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0)
        break;
    }
    close(fbctl);
  }
  int real_w = *w, real_h = *h;
  if (real_w <= 0 || real_w > screen_w)
    real_w = screen_w;
  if (real_h <= 0 || real_h > screen_h)
    real_h = screen_h;
  // canvas_w = real_w;
  // canvas_h = real_h;

  *w = real_w;
  *h = real_h;
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  x += (screen_w - w) / 2;
  y += (screen_h - h) / 2;
  int fd = open("/dev/fb", 0);
  for (int i = 0; i < h; i++) {
    lseek(fd, 4 * (x + (y + i) * screen_w), SEEK_SET);
    write(fd, pixels + i * w, w * 4);
  }
  close(fd);
}

void NDL_OpenAudio(int freq, int channels, int samples) {}

void NDL_CloseAudio() {}

int NDL_PlayAudio(void *buf, int len) { return 0; }

int NDL_QueryAudio() { return 0; }

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  int fd = open("/proc/dispinfo", 0);
  char buf[64];
  size_t nread = read(fd, buf, sizeof(buf) - 1);
  buf[nread] = '\0';
  sscanf(buf, "WIDTH:%d\nHEIGHT:%d\n", &screen_w, &screen_h);
  close(fd);

  return 0;
}

void NDL_Quit() {}
