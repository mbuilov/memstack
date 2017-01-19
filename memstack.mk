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

ifeq (LINUX,$(OS))

include $(MTOP)/LINUX/pc.mk
include $(MTOP)/LINUX/la.mk

define MEMSTACK_DESC
memory stack-allocation helper library
endef

define MEMSTACK_COMMENT
Author:  $(VENDOR_NAME)
License: LGPLv2+
endef

$(call PKGCONFIG_RULE,$(DLL),$(SOVER),$(MEMSTACK_DESC),$(MEMSTACK_COMMENT),$(VENDOR_URL))
$(call LIBTOOL_LA_RULE,$(DLL),$(SOVER),$(LIB))

endif # LINUX

$(DEFINE_TARGETS)
