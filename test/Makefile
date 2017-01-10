include $(dir $(lastword $(MAKEFILE_LIST)))../top.mk
include $(MTOP)/c.mk
EXE     := memstack_test
INCLUDE := ..
SRC     := memstack_test.c
ifdef DEBUG
DEFINES := MEMSTACK_DEBUG
LIBS    := memstackd
else
LIBS    := memstack
endif
$(DEFINE_TARGETS)
