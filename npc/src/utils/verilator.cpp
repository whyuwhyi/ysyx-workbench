#include "Vysyx_25030081_cpu.h"
#include "memory/pmem.h"
#include "cpu/simulator.h"
#include "verilated.h"
#include "verilated_fst_c.h"
#include <nvboard.h>

static VerilatedContext *contextp = NULL;
static VerilatedFstC *tfp = NULL;
static TOP_NAME *top = NULL;

static void step_and_dump_wave() {
  top->eval();

#ifdef _SIMULATE_
  tfp->dump(contextp->time());
  contextp->timeInc(1);
#endif
}

static void reset(int n) {
  top->rst = 1;
  while (n-- > 0)
    single_cycle();
  top->rst = 0;
}

void single_cycle() {
  top->clk = 0;
  step_and_dump_wave();
  top->clk = 1;
  step_and_dump_wave();
}

void run(int n) {
  for (int i = 0; i < n; i++) {
    top->inst = pmem_read(top->pc);
    printf("pc: 0x%08x, inst: 0x%08x\n", top->pc, top->inst);
    single_cycle();
  }
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
  reset(10);
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

extern "C" uint32_t get_pc(void) { return top->pc; }

extern "C" int get_gpr(int idx) {
  if (idx == 10) {
    return top->a0;
  }
  return 0; // 其他寄存器暂时返回0
}
