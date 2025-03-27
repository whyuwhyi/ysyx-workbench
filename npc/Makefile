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

$(shell mkdir -p $(BUILD_DIR))

# project source
VSRCS = $(shell find $(abspath ./vsrc) -name "*.v")
CSRCS = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")

# rules for verilator
INCFLAGS = $(addprefix -I, $(INC_PATH))
CXXFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""

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

NVBOARD_BIN: $(eval include $(NVBOARD_HOME)/scripts/nvboard.mk)
NVBOARD_BIN: CXXFLAGS += -D_NVBOARD_
NVBOARD_BIN: CSRCS += $(SRC_AUTO_BIND)
NVBOARD_BIN: $(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE)

nvboard: $(BIN) $(NVBOARD_ARCHIVE) NVBOARD_BIN
	@$<

run: $(BIN)
	@$^

sim: CXXFLAGS += -D_SIMULATE_
sim: $(BIN)
	@$(call git_commit, "sim RTL")
	@$^
	@$(GTKWAVE) $(WAVE)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: default all clean run sim

include ../Makefile
