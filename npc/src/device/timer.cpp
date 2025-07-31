#include <common.h>
#include <sys/time.h>
#include <time.h>

static uint64_t boot_time = 0;

void timer_init() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  boot_time = tv.tv_sec * 1000 + tv.tv_usec / 1000;
  Log("Timer initialized");
}

int timer_get_timer() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  uint64_t current_time = tv.tv_sec * 1000 + tv.tv_usec / 1000;
  return (int)(current_time - boot_time);
}
