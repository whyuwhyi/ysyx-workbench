#include <common.h>
#include <cpu/cpu.h>
#include <memory/pmem.h>

// Simple DPI-C function implementations
extern "C" bool check_ebreak() { return false; }

extern "C" void reset_npc() {
  // Reset implementation - placeholder
}
