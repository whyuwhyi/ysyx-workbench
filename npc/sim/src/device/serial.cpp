#include <common.h>
#include <defs.h>

void serial_init() { Log("Serial device initialized"); }

void serial_putchar(char c) { putchar(c); }
