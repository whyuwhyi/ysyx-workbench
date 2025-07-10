#include "include/common.h"
#include "include/cpu.h"

bool is_sim_end = false;

extern "C" void ebreak(void) { 
    is_sim_end = true; 
}
