/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NPC is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <dlfcn.h>
#include <common.h>
#include <cpu/cpu.h>
#include <memory/pmem.h>

// RISC-V register state for difftest
typedef struct {
  uint32_t gpr[32];
  uint32_t pc;
} riscv32_CPU_state;

// Function pointers to reference implementation
void (*ref_difftest_memcpy)(uint32_t addr, void *buf, size_t n, bool direction) = NULL;
void (*ref_difftest_regcpy)(void *dut, bool direction) = NULL;
void (*ref_difftest_exec)(uint64_t n) = NULL;
void (*ref_difftest_raise_intr)(uint64_t NO) = NULL;

#ifdef CONFIG_DIFFTEST

static bool is_skip_ref = false;
static int skip_dut_nr_inst = 0;

// Direction constants for difftest
#define DIFFTEST_TO_DUT true
#define DIFFTEST_TO_REF false

// this is used to let ref skip instructions which
// can not produce consistent behavior with NPC
void difftest_skip_ref() {
  is_skip_ref = true;
  skip_dut_nr_inst = 0;
}

// this is used to deal with instruction packing in reference.
void difftest_skip_dut(int nr_ref, int nr_dut) {
  skip_dut_nr_inst += nr_dut;

  while (nr_ref -- > 0) {
    ref_difftest_exec(1);
  }
}

// External functions to get NPC state - will be implemented via DPI-C
extern uint32_t get_npc_pc();
extern uint32_t get_npc_reg(int idx);
extern void set_npc_reg(int idx, uint32_t value);

void init_difftest(char *ref_so_file, long img_size, int port) {
  assert(ref_so_file != NULL);

  void *handle;
  handle = dlopen(ref_so_file, RTLD_LAZY);
  assert(handle);

  ref_difftest_memcpy = (void (*)(uint32_t, void *, size_t, bool))dlsym(handle, "difftest_memcpy");
  assert(ref_difftest_memcpy);

  ref_difftest_regcpy = (void (*)(void *, bool))dlsym(handle, "difftest_regcpy");
  assert(ref_difftest_regcpy);

  ref_difftest_exec = (void (*)(uint64_t))dlsym(handle, "difftest_exec");
  assert(ref_difftest_exec);

  ref_difftest_raise_intr = (void (*)(uint64_t))dlsym(handle, "difftest_raise_intr");
  assert(ref_difftest_raise_intr);

  void (*ref_difftest_init)(int) = (void (*)(int))dlsym(handle, "difftest_init");
  assert(ref_difftest_init);

  Log("Differential testing: ON");
  Log("The result of every instruction will be compared with %s. "
      "This will help you a lot for debugging, but also significantly reduce the performance. "
      "If it is not necessary, you can turn it off in menuconfig.", ref_so_file);

  ref_difftest_init(port);
  
  // Copy initial memory state to reference
  uint8_t *pmem = get_pmem();
  ref_difftest_memcpy(CONFIG_MBASE, pmem, img_size, DIFFTEST_TO_REF);
  
  // Copy initial register state to reference
  riscv32_CPU_state cpu_state;
  for (int i = 0; i < 32; i++) {
    cpu_state.gpr[i] = get_npc_reg(i);
  }
  cpu_state.pc = get_npc_pc();
  ref_difftest_regcpy(&cpu_state, DIFFTEST_TO_REF);
}

static void checkregs(riscv32_CPU_state *ref, uint32_t pc) {
  bool diff_found = false;
  
  // Check PC
  uint32_t dut_pc = get_npc_pc();
  if (ref->pc != dut_pc) {
    Log("PC differs: REF=0x%08x, DUT=0x%08x at pc=0x%08x", ref->pc, dut_pc, pc);
    diff_found = true;
  }
  
  // Check general purpose registers
  for (int i = 0; i < 32; i++) {
    uint32_t dut_reg = get_npc_reg(i);
    if (ref->gpr[i] != dut_reg) {
      Log("Register x%d differs: REF=0x%08x, DUT=0x%08x at pc=0x%08x", 
             i, ref->gpr[i], dut_reg, pc);
      diff_found = true;
    }
  }
  
  if (diff_found) {
    Log("DiffTest failed at pc=0x%08x", pc);
    Log("REF registers:");
    for (int i = 0; i < 32; i++) {
      Log("x%d=0x%08x ", i, ref->gpr[i]);
      if ((i + 1) % 4 == 0) Log("");
    }
    Log("pc=0x%08x", ref->pc);
    
    Log("DUT registers:");
    for (int i = 0; i < 32; i++) {
      Log("x%d=0x%08x ", i, get_npc_reg(i));
      if ((i + 1) % 4 == 0) Log("");
    }
    Log("pc=0x%08x", get_npc_pc());
    
    assert(0);
  }
}

void difftest_step(uint32_t pc, uint32_t npc) {
  riscv32_CPU_state ref_r;

  if (skip_dut_nr_inst > 0) {
    ref_difftest_regcpy(&ref_r, DIFFTEST_TO_DUT);
    if (ref_r.pc == npc) {
      skip_dut_nr_inst = 0;
      checkregs(&ref_r, npc);
      return;
    }
    skip_dut_nr_inst --;
    if (skip_dut_nr_inst == 0)
      panic("can not catch up with ref.pc = 0x%08x at pc = 0x%08x", ref_r.pc, pc);
    return;
  }

  if (is_skip_ref) {
    // to skip the checking of an instruction, just copy the reg state to reference design
    riscv32_CPU_state cpu_state;
    for (int i = 0; i < 32; i++) {
      cpu_state.gpr[i] = get_npc_reg(i);
    }
    cpu_state.pc = get_npc_pc();
    ref_difftest_regcpy(&cpu_state, DIFFTEST_TO_REF);
    is_skip_ref = false;
    return;
  }

  ref_difftest_exec(1);
  ref_difftest_regcpy(&ref_r, DIFFTEST_TO_DUT);

  checkregs(&ref_r, pc);
}

#else

void init_difftest(char *ref_so_file, long img_size, int port) { }
void difftest_step(uint32_t pc, uint32_t npc) { }
void difftest_skip_ref() { }
void difftest_skip_dut(int nr_ref, int nr_dut) { }

#endif