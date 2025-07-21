#include <Vysyx_25030081_cpu.h>
#include <cpu/cpu.h>
#include <memory/pmem.h>
#ifdef CONFIG_NVBOARD
#include <nvboard.h>
#endif
#include <verilated.h>
#include <verilated_fst_c.h>
#include <svdpi.h>


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
  // Ensure DPI-C scope is set before calling DPI functions
  svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.pc_inst"));
  
  for (int i = 0; i < n; i++) {
    // Execute one cycle
    single_cycle();
    
    // Get current PC value using DPI-C
    uint32_t current_pc = get_pc_value();
    uint32_t current_inst = pmem_read(current_pc);
    
    Log("pc: 0x%08x, inst: 0x%08x", current_pc, current_inst);
    
    // Check if ebreak was executed
    if (check_ebreak()) {
      Log("Simulation stopped due to EBREAK");
      break;
    }
  }
}

void sim_init() {
  contextp = new VerilatedContext;
  top = new TOP_NAME{contextp};
  
  // Set DPI-C scope for the PC module where get_pc_value is defined
  svSetScope(svGetScopeFromName("TOP.ysyx_25030081_cpu.pc_inst"));

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
