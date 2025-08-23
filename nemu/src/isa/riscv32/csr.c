#include "local-include/csr.h"
#include <common.h>

word_t riscv_csr[nr_csr];

const char *csr_names[] = {[mepc] = "mepc",
                           [mtvec] = "mtvec",
                           [mstatus] = "mstatus",
                           [mcause] = "macause"};
