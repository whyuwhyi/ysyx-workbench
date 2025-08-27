#include <stdio.h>
#include <sys/time.h>

int main() {
  struct timeval tv;

  struct timeval new_tv;
  gettimeofday(&tv, NULL);
  int tick = 500;
  while (1) {
    gettimeofday(&new_tv, NULL);
    if (new_tv.tv_sec - tv.tv_sec >= 1 ||
        new_tv.tv_usec - tv.tv_usec >= tick * 1000) {
      printf("tick\n");
      tv = new_tv;
    }
  }
}
