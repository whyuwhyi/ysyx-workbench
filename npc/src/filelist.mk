SRCS-y += src/npc-main.cpp src/cpu/cpu.cpp src/cpu/reg_display.cpp src/cpu/verilator_stubs.cpp
DIRS-y += src/cpu src/monitor src/utils src/memory
DIRS-$(CONFIG_TRACE) += src/trace
DIRS-$(CONFIG_DIFFTEST) += src/cpu/difftest
