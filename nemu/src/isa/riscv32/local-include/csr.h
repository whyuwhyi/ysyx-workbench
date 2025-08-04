#ifndef __CSR_H__
#define __CSR_H__
#include <common.h>

#define CSR(i) riscv_csr[i]

enum {
  mstatus = 0x300,
  mtvec = 0x305,
  macause = 0x342,
  mepc = 0x341,
  nr_csr = 1 << 12
};
extern word_t riscv_csr[nr_csr];

static inline void init_csr() {
  riscv_csr[mstatus] = 0x1800;
  Log("CSR initialized: mstatus = 0x1800");
}

static inline const char *csr_name(int addr) {
  extern const char *csr_names[];
  return csr_names[addr];
}

#endif
