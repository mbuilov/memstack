include $(dir $(lastword $(MAKEFILE_LIST)))../top.mk
include $(MTOP)/c.mk

SRC     := memstack.c
INCLUDE := ..

ifndef NO_SHARED
DLL     := $(MEMSTACK_LIB_NAME) $(MEMSTACK_DLL_VARIANTS)
DEFINES := MEMSTACK_EXPORTS=$(DLL_EXPORTS_DEFINE)
ifdef DEBUG
SRC     += memstack_debug.c
DEFINES := MEMSTACK_DEBUG
endif
endif

$(call MAKE_CONTINUE,SRC INCLUDE)

ifndef NO_STATIC
LIB     := $(MEMSTACK_LIB_NAME) $(MEMSTACK_LIB_VARIANTS)
ifdef DEBUG
DEFINES := MEMSTACK_DEBUG
endif
endif

$(DEFINE_TARGETS)

# installation

LIBRARY_NAME := memstack

LIBRARY_HEADERS := \
  memstack_base.h \
  memstack_comn.h \
  memstack.h \
  $(if $(DEBUG),memstack_debug.h)

define MEMSTACK_PC_COMMENT
Author:  $(VENDOR_NAME)
License: $(PRODUCT_LICENCE)
endef

# generate contents of pkg-config .pc-file
# $1 - static or dynamic library name
# $2 - library variant
MEMSTACK_PC_GENERATOR = $(call PKGCONF_DEF_TEMPLATE,$1,$(MODVER),$(PRODUCT_NAME),$(MEMSTACK_PC_COMMENT),$(VENDOR_URL),,,,$(addprefix \
  -D,$(DEFINES))$(call VARIANT_CFLAGS,$2))

LIBRARY_PC_GEN := MEMSTACK_PC_GENERATOR

include $(MTOP)/exts/install_lib.mk
