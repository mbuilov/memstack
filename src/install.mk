include $(dir $(lastword $(MAKEFILE_LIST)))../make/project.mk
include $(MTOP)/defs.mk
include $(MTOP)/exts/install_lib.mk

DLL := $(if $(NO_SHARED),,$(MEMSTACK_LIB_NAME) $(MEMSTACK_DLL_VARIANTS))
LIB := $(if $(NO_STATIC),,$(MEMSTACK_LIB_NAME) $(MEMSTACK_LIB_VARIANTS))

LIBRARY_NAME := memstack
LIBRARY_HDIR := memstack

LIBRARY_HEADERS := $(addprefix $(TOP)/memstack/,\
  memstack_base.h \
  memstack_comn.h \
  memstack.h \
  $(if $(DEBUG),memstack_debug.h))

define MEMSTACK_PC_COMMENT
Author:  $(VENDOR_NAME)
License: $(PRODUCT_LICENCE)
endef

# generate contents of pkg-config .pc-file
# $1 - static or dynamic library name
# $2 - library variant
MEMSTACK_PC_GENERATOR = $(call PKGCONF_DEF_TEMPLATE,$1,$(PRODUCT_VER),$(PRODUCT_NAME),$(MEMSTACK_PC_COMMENT),$(VENDOR_URL),,,,$(addprefix \
  -D,$(DEFINES))$(call VARIANT_CFLAGS,$2))

LIBRARY_PC_GEN := MEMSTACK_PC_GENERATOR

