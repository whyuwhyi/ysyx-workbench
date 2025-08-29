include $(NAVY_HOME)/scripts/riscv/common.mk
CFLAGS  += -march=rv32e -mabi=ilp32e -mno-div  #overwrite
LDFLAGS += -melf32lriscv
