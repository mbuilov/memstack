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

define PC_DESC
memory stack allocation library
endef

define PC_COMMENT
Author:  $(VENDOR_NAME)
License: LGPL version 2.1 or any later version
endef

$(call PKGCONFIG_RULE,$(DLL),$(SOVER),$(PC_DESC),$(PC_COMMENT),$(VENDOR_URL))
$(call LIBTOOL_LA_RULE,$(DLL),$(SOVER),$(LIB))

endif # LINUX

$(DEFINE_TARGETS)

ifeq (LINUX,$(OS))

install_libmemstack uninstall_libmemstack: all
install_libmemstack uninstall_libmemstack: HEADERS := memstack_base.h memstack_comn.h memstack_config.h memstack_debug.h memstack.h
install_libmemstack uninstall_libmemstack: LIB     := $(LIB)
install_libmemstack uninstall_libmemstack: DLL     := $(DLL)
install_libmemstack uninstall_libmemstack: SOVER   := $(SOVER)

install_libmemstack:
	$(INSTALL) -d '$(DESTDIR)$(PREFIX)/include'
	$(INSTALL) -m 644 $(addprefix $(TOP)/,$(HEADERS)) '$(DESTDIR)$(PREFIX)/include'
	$(INSTALL) -d '$(DESTDIR)$(LIBDIR)'
	$(INSTALL) -m 644 $(LIB_DIR)/lib$(LIB).a '$(DESTDIR)$(LIBDIR)'
	$(INSTALL) -m 755 $(LIB_DIR)/lib$(DLL).la '$(DESTDIR)$(LIBDIR)'
	$(INSTALL) -m 755 $(LIB_DIR)/lib$(DLL).so.$(SOVER) '$(DESTDIR)$(LIBDIR)'
	ln -sf$(if $(VERBOSE),v) lib$(DLL).so.$(SOVER) '$(DESTDIR)$(LIBDIR)/lib$(DLL).so'
	$(INSTALL) -d '$(DESTDIR)$(PKG_CONFIG_DIR)'
	$(INSTALL) -m 644 $(LIB_DIR)/lib$(DLL).pc '$(DESTDIR)$(PKG_CONFIG_DIR)'
	$(LDCONFIG) -n$(if $(VERBOSE),v) '$(DESTDIR)$(LIBDIR)'

uninstall_libmemstack:
	rm -f$(if $(VERBOSE),v) \
  $(foreach h,$(HEADERS),'$(DESTDIR)$(PREFIX)/include/$h') \
  '$(DESTDIR)$(LIBDIR)/lib$(LIB).a' \
  '$(DESTDIR)$(LIBDIR)/lib$(DLL).la' \
  '$(DESTDIR)$(LIBDIR)/lib$(DLL).so' \
  '$(DESTDIR)$(LIBDIR)/lib$(DLL).so.$(SOVER)' \
  '$(DESTDIR)$(PKG_CONFIG_DIR)/lib$(DLL).pc'
	$(LDCONFIG) -n$(if $(VERBOSE),v) '$(DESTDIR)$(LIBDIR)'

.PHONY: install_libmemstack uninstall_libmemstack

endif # LINUX
