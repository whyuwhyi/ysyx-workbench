# Trace subsystem source files

# Instruction trace
SRCS-$(CONFIG_ITRACE) += src/trace/itrace/itrace.c

# Memory trace  
SRCS-$(CONFIG_MTRACE) += src/trace/mtrace/mtrace.c

# Function trace
SRCS-$(CONFIG_FTRACE) += src/trace/ftrace/ftrace.c