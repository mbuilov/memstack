include $(dir $(lastword $(MAKEFILE_LIST)))top.mk
include $(MTOP)/c.mk
LIB     := memstack
DLL     := memstack
SRC     := memstack.c
SOVER   := 1
DLL_DEFINES := MEMSTACK_EXPORTS=$(DLL_EXPORTS_DEFINE)
ifdef DEBUG
LIB     := $(LIB)d
DLL     := $(DLL)d
SRC     += memstack_debug.c
DEFINES := MEMSTACK_DEBUG
endif
$(DEFINE_TARGETS)
