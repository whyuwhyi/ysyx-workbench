#include <NDL.h>
#include <stdio.h>

int main() {
  uint32_t start = NDL_GetTicks();
  uint32_t end = 0;
  while (1) {
    end = NDL_GetTicks();
    if (end - start >= 500) {
      printf("tick\n");
      start = end;
    }
  }
}
