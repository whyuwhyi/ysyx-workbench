include $(NAVY_HOME)/scripts/riscv/common.mk
CROSS_COMPILE = riscv64-linux-gnu-
CFLAGS  += -march=rv32g -mabi=ilp32  #overwrite
CFLAGS  += -Wno-implicit-function-declaration
LDFLAGS += -melf32lriscv
