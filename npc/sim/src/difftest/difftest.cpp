#include "common.h"
#include <defs.h>
#include <dlfcn.h>
#include <memory/memory.h>

enum { DIFFTEST_TO_DUT, DIFFTEST_TO_REF };

static inline void difftest_check_value(const char *name, paddr_t pc,
                                        word_t ref, word_t dut) {
  if (ref != dut) {
    Log("%s is different after executing instruction at pc = " FMT_WORD
        ", right = " FMT_WORD ", wrong = " FMT_WORD ", diff = " FMT_WORD,
        name, pc, ref, dut, ref ^ dut);
    npc_reg_display();
    assert(0);
  }
}

#define DIFFTEST_CHECK_CSR(name, field)                                        \
  difftest_check_value(name##_NAME, pc, ref->csr.field, get_npc_csr(name))

typedef struct {
  word_t mstatus;
  word_t mtvec;
  word_t mepc;
  word_t mcause;
} RISCV_csr;

typedef struct {
  word_t gpr[32];
  RISCV_csr csr;
  paddr_t pc;
} riscv32_CPU_state;

static riscv32_CPU_state dut_riscv32_cpu_state;
static riscv32_CPU_state ref_riscv32_cpu_state;

static void checkregs(riscv32_CPU_state *ref, paddr_t pc);
static void difftest_dut2ref();

bool ref_skip_difftest = false;
static bool last_skip_difftest = false;

void (*ref_difftest_memcpy)(paddr_t addr, void *buf, size_t n,
                            bool direction) = NULL;
void (*ref_difftest_regcpy)(void *dut, bool direction) = NULL;
void (*ref_difftest_exec)(uint64_t n) = NULL;
void (*ref_difftest_raise_intr)(uint64_t NO) = NULL;

void init_difftest(char *ref_so_file, long img_size, int port) {
  assert(ref_so_file != NULL);

  void *handle;
  handle = dlopen(ref_so_file, RTLD_LAZY);
  assert(handle);

  ref_difftest_memcpy =
      (void (*)(paddr_t, void *, size_t, bool))dlsym(handle, "difftest_memcpy");
  assert(ref_difftest_memcpy);

  ref_difftest_regcpy =
      (void (*)(void *, bool))dlsym(handle, "difftest_regcpy");
  assert(ref_difftest_regcpy);

  ref_difftest_exec = (void (*)(uint64_t))dlsym(handle, "difftest_exec");
  assert(ref_difftest_exec);

  ref_difftest_raise_intr =
      (void (*)(uint64_t))dlsym(handle, "difftest_raise_intr");
  assert(ref_difftest_raise_intr);

  void (*ref_difftest_init)(int) =
      (void (*)(int))dlsym(handle, "difftest_init");
  assert(ref_difftest_init);

  Log("Differential testing: ON");
  Log("The result of every instruction will be compared with %s. "
      "This will help you a lot for debugging, but also significantly reduce "
      "the performance. "
      "If it is not necessary, you can turn it off in menuconfig.",
      ref_so_file);

  ref_difftest_init(port);

  extern uint8_t pmem[];
  ref_difftest_memcpy(CONFIG_MBASE, pmem, img_size, DIFFTEST_TO_REF);

  difftest_dut2ref();
}

void difftest_step(paddr_t pc) {
  if (last_skip_difftest) {
    difftest_dut2ref();
    last_skip_difftest = false;
    return;
  }

  if (ref_skip_difftest) {
    ref_skip_difftest = false;
    last_skip_difftest = true;
  }

  ref_difftest_exec(1);
  ref_difftest_regcpy(&ref_riscv32_cpu_state, DIFFTEST_TO_DUT);
  checkregs(&ref_riscv32_cpu_state, pc);
}

static void checkregs(riscv32_CPU_state *ref, paddr_t pc) {
  difftest_check_value("pc", pc, ref->pc, get_npc_pc());
  for (int i = 0; i < nr_reg; i++) {
    difftest_check_value(npc_reg_name(i), pc, ref->gpr[i], get_npc_reg(i));
  }
  DIFFTEST_CHECK_CSR(MSTATUS, mstatus);
  DIFFTEST_CHECK_CSR(MTVEC, mtvec);
  DIFFTEST_CHECK_CSR(MEPC, mepc);
  DIFFTEST_CHECK_CSR(MCAUSE, mcause);
}

static inline void difftest_dut2ref() {
  for (int i = 0; i < nr_reg; i++) {
    dut_riscv32_cpu_state.gpr[i] = get_npc_reg(i);
  }
  dut_riscv32_cpu_state.pc = get_npc_pc();
  dut_riscv32_cpu_state.csr.mstatus = get_npc_csr(MSTATUS);
  dut_riscv32_cpu_state.csr.mtvec = get_npc_csr(MTVEC);
  dut_riscv32_cpu_state.csr.mepc = get_npc_csr(MEPC);
  dut_riscv32_cpu_state.csr.mcause = get_npc_csr(MCAUSE);
  ref_difftest_regcpy(&dut_riscv32_cpu_state, DIFFTEST_TO_REF);
}
