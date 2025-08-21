SRCS-$(CONFIG_DIFFTEST) += src/difftest/difftest.cpp
ARGS += --diff=sim/$(CONFIG_DIFFTEST_REF_PATH)
