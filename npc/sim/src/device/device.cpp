#include <common.h>
#include <defs.h>
#include <device/device.h>

void init_device() {
  serial_init();
  timer_init();
  Log("Device initialization complete");
}
