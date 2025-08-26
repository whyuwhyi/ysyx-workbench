#include <defs.h>
#include <svdpi.h>

static const char *regs[] = {
    "zero", "ra", "sp", "gp", "tp",  "t0",  "t1", "t2", "s0", "s1", "a0",
    "a1",   "a2", "a3", "a4", "a5",  "a6",  "a7", "s2", "s3", "s4", "s5",
    "s6",   "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

const char *npc_reg_name(int index) { return regs[index]; }

static inline void riscv_csr_display(int addr, const char *name) {
  printf("%-11s %-11d " FMT_WORD "\n", name, (int32_t)get_npc_csr(addr),
         get_npc_csr(addr));
}

#define DISPLAY_CSR(name) riscv_csr_display(name, name##_NAME)

void npc_reg_display() {
  printf("Name        Dec         Hex        \n");
  printf("-----General Purpose Registers-----\n");
  for (int i = 0; i < nr_reg; ++i) {
    printf("%-11s %-11d " FMT_WORD "\n", regs[i], (sword_t)get_npc_reg(i),
           get_npc_reg(i));
  }

  printf("----Control and Status Registers----\n");
  DISPLAY_CSR(MSTATUS);
  DISPLAY_CSR(MTVEC);
  DISPLAY_CSR(MEPC);
  DISPLAY_CSR(MCAUSE);

  printf("----------Program Counter-----------\n");
  printf("%-11s %-11d " FMT_WORD "\n", "pc", (sword_t)get_npc_pc(),
         get_npc_pc());
}

#define CSR2VAL(name)                                                          \
  if (strcmp(name##_NAME, s) == 0) {                                           \
    *success = true;                                                           \
    return get_npc_csr(name);                                                  \
  }

word_t npc_reg_str2val(const char *s, bool *success) {
  for (int i = 0; i < 32; ++i) {
    if (strcmp(regs[i], s) == 0) {
      *success = true;
      return get_npc_reg(i);
    }
  }

  CSR2VAL(MSTATUS)
  CSR2VAL(MTVEC)
  CSR2VAL(MEPC)
  CSR2VAL(MCAUSE)

  if (strcmp("pc", s) == 0) {
    *success = true;
    return get_npc_pc();
  }

  *success = false;
  return 0;
}
