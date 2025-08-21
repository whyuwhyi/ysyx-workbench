ifeq ($(CONFIG_ITRACE),y)
LIBCAPSTONE = tools/capstone/repo/libcapstone.so.5
CFLAGS += -I tools/capstone/repo/include
SRCS-y += src/utils/disasm.cpp
src/utils/disasm.cpp: $(LIBCAPSTONE)
$(LIBCAPSTONE):
	$(MAKE) -C tools/capstone
endif
SRCS-y += src/utils/logs.cpp
