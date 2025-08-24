#ifndef __CSR_H__
#define __CSR_H__
#include <common.h>
#include <isa.h>

#define MSTATUS 0x300
#define MTVEC 0x305
#define MEPC 0x341
#define MCAUSE 0x342

static inline word_t *get_csr_ptr(int addr) {
  switch (addr) {
  case MSTATUS:
    return &cpu.csr.mstatus;
  case MTVEC:
    return &cpu.csr.mtvec;
  case MEPC:
    return &cpu.csr.mepc;
  case MCAUSE:
    return &cpu.csr.mcause;
  default:
    Assert(0, "Unimplemented CSR address: 0x%x", addr);
    return NULL;
  }
}

#define csr(i) (*get_csr_ptr(i))

static inline void init_csr() { csr(MSTATUS) = 0x1800; }

static inline const char *csr_name(int addr) {
  extern const char *csr_names[];
  return csr_names[addr];
}

#endif
