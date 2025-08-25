#include <common.h>

void init_etrace() { Log("Exception trace initialized"); }

void etrace_call(paddr_t epc) { Log("Exception at " FMT_PADDR ".", epc); }

void etrace_ret() { Log("Return from exception."); }
