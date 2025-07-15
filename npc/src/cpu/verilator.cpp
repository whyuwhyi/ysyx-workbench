#include <Vysyx_25030081_cpu.h>
#include <cpu/simulator.h>
#include <memory/pmem.h>
#ifdef CONFIG_NVBOARD
#include <nvboard.h>
#endif
#include <verilated.h>
#include <verilated_fst_c.h>

static VerilatedContext *contextp = NULL;
static VerilatedFstC *tfp = NULL;
static TOP_NAME *top = NULL;

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

void cpu_exec(int n) {
  for (int i = 0; i < n; i++) {
    top->inst = pmem_read(top->pc);
    printf("pc: 0x%08x, inst: 0x%08x\n", top->pc, top->inst);
    single_cycle();
  }
}

void sim_init() {
  contextp = new VerilatedContext;
  top = new TOP_NAME{contextp};

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
