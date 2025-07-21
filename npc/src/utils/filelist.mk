SRCS-y += src/utils/timer.cpp

# Disassembly support for instruction trace
ifeq ($(CONFIG_ITRACE),y)
LIBCAPSTONE = tools/capstone/repo/libcapstone.so.5
CFLAGS += -I tools/capstone/repo/include
SRCS-y += src/utils/disasm.cpp
src/utils/disasm.cpp: $(LIBCAPSTONE)
$(LIBCAPSTONE):
	$(MAKE) -C tools/capstone
endif
