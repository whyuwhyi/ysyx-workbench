#include "Vysyx_25030081_cpu.h"
#include "inst/inst.h"
#include "mem/mem.h"
#include "verilated.h"
#include "verilated_fst_c.h"
#include <cassert>
#include <cstdio>
#include <nvboard.h>

#define NR_INST 100

bool is_sim_end = false;

VerilatedContext *contextp = NULL;
VerilatedFstC *tfp = NULL;
TOP_NAME *top = NULL;
word_t res[NR_INST];

void load_inst();
void single_cycle();
void step_and_dump_wave();
void reset(int n);
void sim_init();
void sim_exit();

int main(int argc, char **argv) {
  sim_init();

  reset(10);
  load_inst();
  for (word_t i = 0; i < NR_INST; i++) {
    top->inst = pmem_read(top->pc);
    single_cycle();

    if (is_sim_end)
      break;

    printf("alu_out = 0x%08x \t res = 0x%08x\n", top->alu_out, res[i]);
    assert(top->pc == 0x80000000 + 4 * (i + 1));
    assert(top->alu_out == res[i]);
  }

  sim_exit();
  return 0;
}

void load_inst() {
  srand(time(NULL));
  paddr_t start = 0x80000000;

  for (int i = 0; i < NR_INST; i++) {
    word_t imm = rand() & 0xfff;
    addi(start, 0, imm, 0);
    res[i] = (int32_t)((int16_t)(imm << 4)) >> 4;
    start += 4;
  }
  ebreak(start);
}

void single_cycle() {
  top->clk = 0;
  step_and_dump_wave();
  top->clk = 1;
  step_and_dump_wave();
}

void reset(int n) {
  top->rst = 1;
  while (n-- > 0)
    single_cycle();
  top->rst = 0;
}

void sim_init() {
  contextp = new VerilatedContext;
  top = new TOP_NAME{contextp};

#ifdef _SIMULATE_
  tfp = new VerilatedFstC;
  contextp->traceEverOn(true);
  top->trace(tfp, 0);
  tfp->open("build/wave.fst");
#endif
}

void sim_exit() {
  single_cycle();

#ifdef _SIMULATE_
  tfp->close();
  delete tfp;
#endif

  delete top;
  delete contextp;
}

void step_and_dump_wave() {
  top->eval();

#ifdef _SIMULATE_
  tfp->dump(contextp->time());
  contextp->timeInc(1);
#endif
}
