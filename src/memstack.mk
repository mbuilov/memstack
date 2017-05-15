include $(dir $(lastword $(MAKEFILE_LIST)))../project.mk
include $(MTOP)/c.mk

INCLUDE := ..
SRC     := memstack.c
ifdef DEBUG
SRC     += memstack_debug.c
DEFINES := MEMSTACK_DEBUG
endif

ifndef NO_STATIC
LIB     := $(MEMSTACK_LIB_NAME) $(MEMSTACK_LIB_VARIANTS)
$(call MAKE_CONTINUE,INCLUDE SRC DEFINES)
endif

ifndef NO_SHARED
DLL     := $(MEMSTACK_LIB_NAME) $(MEMSTACK_DLL_VARIANTS)
DEFINES += MEMSTACK_EXPORTS=$(DLL_EXPORTS_DEFINE)
endif

$(DEFINE_TARGETS)
