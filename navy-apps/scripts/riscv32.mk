include $(NAVY_HOME)/scripts/riscv/common.mk
CROSS_COMPILE = riscv64-linux-gnu-
CFLAGS  += -march=rv32g -mabi=ilp32  #overwrite
LDFLAGS += -melf32lriscv
