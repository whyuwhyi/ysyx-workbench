#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "verilated.h"
#include "Vswitch.h"
#include "verilated_fst_c.h"


int main(int argc, char **argv) {
  VerilatedContext *contextp = new VerilatedContext;
  Vswitch* top= new Vswitch;
  VerilatedFstC* tfp = new VerilatedFstC;

  contextp->commandArgs(argc, argv);
  contextp->traceEverOn(true);
  top->trace(tfp, 0);
  tfp->open("wave.fst");

  top->a = 0;
  top->b = 0;

  int i;
  for (i = 0; i < 10; i++) {
    int a = rand() & 1;
    int b = rand() & 1;
    top->a = a;
    top->b = b;
    top->eval();
    printf("a = %d, b = %d, f = %d\n", top->a, top->b, top->f);
    assert(top->f == (a ^ b));

    tfp->dump(contextp->time());
    contextp->timeInc(1);
  }

  delete top;
  tfp->close();
  delete tfp;
  delete contextp;

  return 0;
}
