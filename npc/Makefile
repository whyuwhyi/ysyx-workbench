TOPNAME = ysyx_25030081_cpu
NXDC_FILES = constr/top.nxdc
INC_PATH ?=
IMG ?= 

VERILATOR = verilator
VERILATOR_CFLAGS += -MMD --build -cc  \
				-O3 --x-assign fast --x-initial fast --noassert

GTKWAVE = gtkwave
WAVE = $(BUILD_DIR)/wave.fst

BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOPNAME)

default: $(BIN)

# project source
VSRCS = $(shell find $(abspath ./vsrc) -name "*.v")
CSRCS = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")

# rules for verilator
INCFLAGS = $(addprefix -I, $(INC_PATH)) -I$(abspath ./csrc)
CXXFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BIN): $(VSRCS) $(CSRCS) | $(BUILD_DIR)
	@rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_CFLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--trace-fst \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))

# constraint file
SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES) | $(BUILD_DIR)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

# nvboard build with specific flags
NVBOARD_CXXFLAGS = $(CXXFLAGS) -D_NVBOARD_
NVBOARD_CSRCS = $(CSRCS) $(SRC_AUTO_BIND)

nvboard: $(NVBOARD_ARCHIVE) $(SRC_AUTO_BIND) $(VSRCS) $(NVBOARD_CSRCS) | $(BUILD_DIR)
	@rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_CFLAGS) \
		--top-module $(TOPNAME) $(VSRCS) \
		$(addprefix -CFLAGS , $(NVBOARD_CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--trace-fst \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN)) \
		$(NVBOARD_CSRCS)
	@$(BIN)

# simulation build with specific flags
SIM_CXXFLAGS = $(CXXFLAGS) -D_SIMULATE_
SIM_BIN = $(BUILD_DIR)/$(TOPNAME)_sim

$(SIM_BIN): $(VSRCS) $(CSRCS) | $(BUILD_DIR)
	@rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_CFLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(SIM_CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--trace-fst \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(SIM_BIN))

sim: $(SIM_BIN)
	@$(call git_commit, "sim RTL")
	@$(SIM_BIN) $(IMG)
	@$(GTKWAVE) $(WAVE)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: default clean sim nvboard

include ../Makefile

ifneq ($(NVBOARD_HOME),)
include $(NVBOARD_HOME)/scripts/nvboard.mk
endif
