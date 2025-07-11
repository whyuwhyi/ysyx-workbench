# Trace subsystem source files

# Include trace subdirectories conditionally
DIRS-$(CONFIG_ITRACE) += src/trace/itrace
DIRS-$(CONFIG_MTRACE) += src/trace/mtrace
DIRS-$(CONFIG_FTRACE) += src/trace/ftrace