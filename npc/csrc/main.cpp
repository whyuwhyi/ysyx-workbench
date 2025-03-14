#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <nvboard.h>
#include "verilated.h"
#include "Vlight.h"
#include "verilated_fst_c.h"

static TOP_NAME *top;

void single_cycle();
void reset(int n);
void nvboard_bind_all_pins(TOP_NAME* top);

int main(int argc, char **argv) {
  VerilatedContext *contextp = new VerilatedContext;
  contextp->commandArgs(argc, argv);
  contextp->traceEverOn(true);

  VerilatedFstC* tfp = new VerilatedFstC;
  top = new TOP_NAME(contextp);
  top->trace(tfp, 0);
  tfp->open("./build/wave.fst");
  
  nvboard_bind_all_pins(top);
  nvboard_init();

  reset(10);

  while (1) {
    nvboard_update();
    single_cycle();
    tfp->dump(contextp->time());
    contextp->timeInc(1);
  }

  delete top;
  tfp->close();
  delete tfp;
  delete contextp;

  return 0;
}

void single_cycle() {
  top->clk = 0; top->eval();
  top->clk = 1; top->eval();
}

void reset(int n) {
  top->rst = 1;
  while (n -- > 0) single_cycle();
  top->rst = 0;
}


