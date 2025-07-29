#include "Vysyx_25030081_cpu.h"
#include <Vysyx_25030081_cpu.h>
#include <cpu/cpu.h>
#include <memory/pmem.h>
#include <svdpi.h>
#include <verilated.h>
#include <verilated_fst_c.h>
#ifdef CONFIG_NVBOARD
#include <nvboard.h>
#endif

static VerilatedContext *contextp = NULL;
static VerilatedFstC *tfp = NULL;
static Vysyx_25030081_cpu *top = NULL;

static void step_and_dump_wave() {
  top->eval();

#ifdef CONFIG_WAVE_TRACE
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

void sim_init() {
  contextp = new VerilatedContext;
  top = new Vysyx_25030081_cpu{contextp};

  // svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.pc_inst"));

#ifdef CONFIG_WAVE_TRACE
  tfp = new VerilatedFstC;
  contextp->traceEverOn(true);
  top->trace(tfp, 0);
  tfp->open("build/wave.fst");
#endif
  reset(100);
}

void sim_exit() {
  single_cycle();

#ifdef CONFIG_WAVE_TRACE
  tfp->close();
  delete tfp;
#endif

  delete top;
  delete contextp;
}
