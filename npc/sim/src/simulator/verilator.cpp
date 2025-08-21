#include <VCore.h>
#include <defs.h>
#include <verilated.h>
#include <verilated_fst_c.h>
#ifdef CONFIG_NVBOARD
#include <nvboard.h>
#endif

#define RESET (top->reset)
#define CLOCK (top->clock)

static VerilatedContext *contextp = NULL;
static VerilatedFstC *tfp = NULL;
static VCore *top = NULL;

static void step_and_dump_wave() {
  top->eval();

#ifdef CONFIG_WAVE_TRACE
  tfp->dump(contextp->time());
  contextp->timeInc(1);
#endif
}

static void reset(int n) {
  RESET = 1;
  while (n-- > 0)
    single_cycle();
  RESET = 0;
  top->eval();
}

void single_cycle() {
  CLOCK = 0;
  step_and_dump_wave();
  CLOCK = 1;
  step_and_dump_wave();
}

void sim_init() {
  contextp = new VerilatedContext;
  top = new VCore{contextp};

#ifdef CONFIG_WAVE_TRACE
  tfp = new VerilatedFstC;
  contextp->traceEverOn(true);
  top->trace(tfp, 0);
  tfp->open("build/wave.fst");
#endif
  reset(10);
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
