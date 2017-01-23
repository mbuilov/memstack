include $(dir $(lastword $(MAKEFILE_LIST)))../top.mk
include $(MTOP)/c.mk

EXE     := memstack_test_c
INCLUDE := ..
SRC     := memstack_test.c

ifdef DEBUG
DEFINES := MEMSTACK_DEBUG
LIBS    := memstackd
else
LIBS    := memstack
endif

ifneq ($(filter check,$(MAKECMDGOALS)),)
$(call ADD_GENERATED,$(BIN_DIR)/$(EXE).out)
$(BIN_DIR)/$(EXE).out: $(call FORM_TRG,EXE)
	$(call SUP,TEST,$@)$< 2> $@
endif

$(call MAKE_CONTINUE,INCLUDE DEFINES LIBS)

EXE     := memstack_test_cpp
SRC     := memstack_test.cpp

ifneq ($(filter check,$(MAKECMDGOALS)),)
$(call ADD_GENERATED,$(BIN_DIR)/$(EXE).out)
$(BIN_DIR)/$(EXE).out: $(call FORM_TRG,EXE)
	$(call SUP,TEST,$@)$< 2> $@
endif

$(DEFINE_TARGETS)
