#include "local-include/csr.h"
#include <common.h>

word_t csr[nr_csr];

const char *csr_names[] = {
    [mepc] = "mepc",
    [mstatus] = "mstatus",
    [macause] = "macause",
};
