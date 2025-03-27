TOPNAME = ysyx_25020081_cpu
NXDC_FILES = constr/top.nxdc
INC_PATH ?=

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
INCFLAGS = $(addprefix -I, $(INC_PATH))
CXXFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""

# $(BIN): $(shell mkdir -p $(BUILD_DIR))
$(BIN): $(VSRCS) $(CSRCS)
	@rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_CFLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--trace-fst \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))

# constraint file
SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@


nvboard: $(shell rm -rf build)
nvboard: $(shell mkdir -p $(BUILD_DIR))
nvboard: $(eval include $(NVBOARD_HOME)/scripts/nvboard.mk)
nvboard: CXXFLAGS += -D_NVBOARD_
nvboard: CSRCS += $(SRC_AUTO_BIND)

nvboard: $(NVBOARD_ARCHIVE) $(BIN)
	@$(BIN)

run: $(BIN)
	@$^

sim: $(shell rm -rf build)
sim: $(shell mkdir -p $(BUILD_DIR))
sim: CXXFLAGS += -D_SIMULATE_
sim: $(BIN)
	@$(call git_commit, "sim RTL")
	@$(BIN)
	@$(GTKWAVE) $(WAVE)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: default all clean run sim

include ../Makefile
