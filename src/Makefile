include $(dir $(lastword $(MAKEFILE_LIST)))../top.mk
include $(MTOP)/c.mk

LIB := $(if $(NO_STATIC),,$(MEMSTACK_LIB_NAME) $(MEMSTACK_LIB_VARIANTS))
DLL := $(if $(NO_SHARED),,$(MEMSTACK_LIB_NAME) $(MEMSTACK_DLL_VARIANTS))

SRC         := memstack.c
INCLUDE     := ..
DLL_DEFINES := MEMSTACK_EXPORTS=$(DLL_EXPORTS_DEFINE)

ifdef DEBUG
SRC     += memstack_debug.c
DEFINES := MEMSTACK_DEBUG
endif

$(DEFINE_TARGETS)

include $(TOP)/src/install.mk
