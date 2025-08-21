# Check if .config exists
# ifeq ($(wildcard $(NPC_HOME)/sim/.config),)
# $(error Error: .config does not exist!)
# $(error To build the project, first run 'make menuconfig'.)
# endif
#
# Project roots
PROJ_ROOT    := $(abspath $(shell pwd))
SIM_DIR      := $(abspath $(PROJ_ROOT)/sim)

-include $(SIM_DIR)/include/config/auto.conf
-include $(SIM_DIR)/include/config/auto.conf.cmd

remove_quote = $(patsubst "%",%,$(1))

# NPC configuration
TOPNAME = Core
NAME    = $(TOPNAME)
IMG    ?= ../am-kernels/tests/cpu-tests/build/dummy-riscv32e-npc.bin

# Flags derived from config
CFLAGS_TRACE += -DITRACE_COND=$(if $(CONFIG_ITRACE_COND),$(call remove_quote,$(CONFIG_ITRACE_COND)),true)
CFLAGS_BUILD += $(if $(CONFIG_WAVE_TRACE),-DCONFIG_WAVE_TRACE,)
CFLAGS_BUILD += $(if $(CONFIG_NVBOARD),-DCONFIG_NVBOARD,)

# Verilator configuration
VERILATOR = verilator
VERILATOR_CFLAGS += -MMD --build -cc -O3 --x-assign fast --x-initial fast --noassert --quiet-exit -I$(PROJ_ROOT)/rtl/include
BUILD_DIR = $(PROJ_ROOT)/build
OBJ_DIR   = $(BUILD_DIR)/obj_dir
TARGET    = $(BUILD_DIR)/$(TOPNAME)

$(shell mkdir -p $(BUILD_DIR))

# Include paths
INC_PATH   = $(SIM_DIR)/include
INCFLAGS   = $(addprefix -I, $(abspath $(INC_PATH)))
CXXFLAGS  += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\"" -ffile-prefix-map=$(abspath .)/= -g
CFLAGS    += $(CFLAGS_BUILD) $(CFLAGS_TRACE)
LDFLAGS   += $(CFLAGS_BUILD)

# Filelists from sim/src
FILELIST_MK = $(shell find -L $(SIM_DIR)/src -name "filelist.mk")
include $(FILELIST_MK)

# Sources
SRCS_DIRS := $(shell find -L $(addprefix $(SIM_DIR)/,$(DIRS-y)) -name "*.c" -o -name "*.cpp" 2>/dev/null || true)
SRCS_LIST := $(shell find -L $(addprefix $(SIM_DIR)/,$(SRCS-y)) -name "*.c" -o -name "*.cpp" 2>/dev/null || true)

SRCS := $(abspath $(SRCS_LIST) $(SRCS_DIRS))

# Libraries
LIBS :=
LIBS += -lreadline
LIBS += $(if $(CONFIG_DIFFTEST),-ldl)
LIBS += $(if $(CONFIG_ITRACE),-ldl)

# RTL sources: sanitize a copy of Core.sv
CORE_SV_SRC := $(PROJ_ROOT)/rtl/Core.sv
SAN_CORE    := $(BUILD_DIR)/Core.sv
BBX_MARKER  := ^\/\/ ----- 8< ----- FILE "firrtl_black_box_resource_files\.f" ----- 8< -----

$(SAN_CORE): $(CORE_SV_SRC)
	@mkdir -p $(BUILD_DIR)
	@cp $(CORE_SV_SRC) $(SAN_CORE)
	@sed -i -e '/$(BBX_MARKER)/,$$d' $(SAN_CORE) || true

VSRCS := $(SAN_CORE)

# Default target
.DEFAULT_GOAL := $(TARGET)

$(TARGET): $(VSRCS) $(SRCS) $(if $(CONFIG_NVBOARD),$(SRC_AUTO_BIND))
	@$(VERILATOR) $(VERILATOR_CFLAGS) \
		--top-module $(TOPNAME) $(VSRCS) $(SRCS) $(if $(CONFIG_NVBOARD),$(SRC_AUTO_BIND)) \
		$(addprefix -CFLAGS , $(CXXFLAGS)) \
		$(addprefix -LDFLAGS , $(LDFLAGS) $(LIBS)) \
		--trace-fst \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(TARGET))

# NVBoard integration
ifneq ($(NVBOARD_HOME),)
include $(NVBOARD_HOME)/scripts/nvboard.mk

NXDC_FILES    = $(SIM_DIR)/constr/top.nxdc
SRC_AUTO_BIND = $(abspath ./build/auto_bind.cpp)

$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

ifeq ($(CONFIG_NVBOARD),y)
SRCS += $(SRC_AUTO_BIND)
LIBS += $(NVBOARD_ARCHIVE)
endif
endif

# Default arguments for simulation
override ARGS ?= --log=$(BUILD_DIR)/npc-log.txt

ifdef CONFIG_DIFFTEST
DIFF_REF_SO = $(NPC_HOME)/sim/$(call remove_quote,$(CONFIG_DIFFTEST_REF_PATH))
ARGS_DIFF = --diff=$(DIFF_REF_SO)
endif

# Simulation target
sim: $(TARGET)
	@$(call git_commit, "sim RTL")
	$(TARGET) $(ARGS) $(IMG)

# Wave viewer
GTKWAVE = gtkwave
WAVE    = $(BUILD_DIR)/wave.fst

wave: $(WAVE)
	@$(GTKWAVE) $(WAVE)

clean:
	@rm -rf $(BUILD_DIR)

menuconfig:
	@make -C $(SIM_DIR) menuconfig

# Build alias
build: $(TARGET)

include ../Makefile

.PHONY: build sim wave clean

