SRCS-y += src/npc-main.cpp src/cpu/cpu.cpp src/monitor/sdb/reg_display.cpp src/simulator/verilator.cpp
DIRS-y += src/cpu src/monitor src/utils src/memory
DIRS-$(CONFIG_TRACE) += src/trace
DIRS-$(CONFIG_DIFFTEST) += src/cpu/difftest
