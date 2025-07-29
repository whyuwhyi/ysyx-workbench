#include "Vysyx_25030081_cpu__Dpi.h"
#include <common.h>
#include <cpu/cpu.h>
#include <dlfcn.h>
#include <memory/pmem.h>

// Direction constants for difftest
#define DIFFTEST_TO_DUT true
#define DIFFTEST_TO_REF false

typedef struct {
  uint32_t gpr[16];
  uint32_t pc;
} riscv32_CPU_state;

static riscv32_CPU_state dut_riscv32_cpu_state;

// Function pointers to reference implementation
void (*ref_difftest_memcpy)(uint32_t addr, void *buf, size_t n,
                            bool direction) = NULL;
void (*ref_difftest_regcpy)(void *dut, bool direction) = NULL;
void (*ref_difftest_exec)(uint64_t n) = NULL;
void (*ref_difftest_raise_intr)(uint64_t NO) = NULL;

void init_difftest(char *ref_so_file, long img_size, int port) {
  assert(ref_so_file != NULL);

  void *handle;
  handle = dlopen(ref_so_file, RTLD_LAZY);
  assert(handle);

  ref_difftest_memcpy = (void (*)(uint32_t, void *, size_t, bool))dlsym(
      handle, "difftest_memcpy");
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

  // Copy initial memory state to reference
  uint8_t *pmem = get_pmem();
  ref_difftest_memcpy(CONFIG_MBASE, pmem, img_size, DIFFTEST_TO_REF);

  for (int i = 0; i < 32; i++) {
    dut_riscv32_cpu_state.gpr[i] = get_npc_reg(i);
  }
  dut_riscv32_cpu_state.pc = get_npc_pc();
  ref_difftest_regcpy(&dut_riscv32_cpu_state, DIFFTEST_TO_REF);
}

static void checkregs(riscv32_CPU_state *ref, uint32_t pc) {
  bool diff_found = false;

  uint32_t dut_pc = get_npc_pc();
  if (ref->pc != dut_pc) {
    Log("PC differs: REF=0x%08x, DUT=0x%08x at pc=0x%08x", ref->pc, dut_pc, pc);
    diff_found = true;
  }

  // Check general purpose registers
  for (int i = 0; i < 32; i++) {
    uint32_t dut_reg = get_npc_reg(i);
    if (ref->gpr[i] != dut_reg) {
      Log("Register x%d differs: REF=0x%08x, DUT=0x%08x at pc=0x%08x", i,
          ref->gpr[i], dut_reg, pc);
      diff_found = true;
    }
  }

  if (diff_found) {
    Log("DiffTest failed at pc=0x%08x", pc);
    Log("REF registers:");
    for (int i = 0; i < 32; i++) {
      Log("x%d=0x%08x ", i, ref->gpr[i]);
      if ((i + 1) % 4 == 0)
        Log("");
    }
    Log("pc=0x%08x", ref->pc);

    Log("DUT registers:");
    for (int i = 0; i < 32; i++) {
      Log("x%d=0x%08x ", i, get_npc_reg(i));
      if ((i + 1) % 4 == 0)
        Log("");
    }
    Log("pc=0x%08x", get_npc_pc());

    assert(0);
  }
}

void difftest_step(uint32_t pc, uint32_t npc) {
  riscv32_CPU_state ref_r;

  ref_difftest_exec(1);
  ref_difftest_regcpy(&ref_r, DIFFTEST_TO_DUT);

  checkregs(&ref_r, pc);
}
