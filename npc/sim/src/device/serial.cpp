#include <defs.h>

void serial_putchar(char c) {
  putchar(c);
  fflush(stdout);
}
