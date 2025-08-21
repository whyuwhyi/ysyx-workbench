SRCS-$(CONFIG_DIFFTEST) += src/difftest/difftest.cpp

ifdef CONFIG_DIFFTEST
DIFF_REF_SO = $(NPC_HOME)/sim/$(call remove_quote,$(CONFIG_DIFFTEST_REF_PATH))
ARGS_DIFF = --diff=$(DIFF_REF_SO)
endif
