NAME = libgcc

# Only include these sources for RISC-V32E
ifeq ($(ISA),riscv32e)
SRCS = div.S \
       muldi3.S \
       multi3.c \
       ashldi3.c 
endif

include $(NAVY_HOME)/Makefile
