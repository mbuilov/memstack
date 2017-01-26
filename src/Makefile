include $(dir $(lastword $(MAKEFILE_LIST)))../top.mk
include $(MTOP)/c.mk

LIB     := memstack
DLL     := memstack
SRC     := memstack.c
INCLUDE := ..
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

PC_CFLAGS := -I$${includedir}/memstack

$(call PKGCONFIG_RULE,$(DLL),$(SOVER),$(PC_DESC),$(PC_COMMENT),$(VENDOR_URL))
$(call LIBTOOL_LA_RULE,$(DLL),$(SOVER),$(LIB))

endif # LINUX

$(DEFINE_TARGETS)

install_libmemstack uninstall_libmemstack: LIB   := $(LIB)
install_libmemstack uninstall_libmemstack: DLL   := $(DLL)
install_libmemstack uninstall_libmemstack: SOVER := $(SOVER)

install_libmemstack: HEADERS := \
  memstack_base.h \
  memstack_comn.h \
  memstack_debug.h \
  memstack.h

install_libmemstack: $(call FORM_TRG,LIB) $(call FORM_TRG,DLL)

ifeq (LINUX,$(OS))

install_libmemstack: $(LIB_DIR)/lib$(DLL).so.$(SOVER) $(LIB_DIR)/lib$(DLL).la  $(LIB_DIR)/lib$(DLL).pc
	$(INSTALL) -d '$(DESTDIR)$(PREFIX)/include/memstack'
	$(INSTALL) -m 644 $(addprefix $(TOP)/memstack/,$(HEADERS)) '$(DESTDIR)$(PREFIX)/include/memstack'
	$(INSTALL) -d '$(DESTDIR)$(LIBDIR)'
	$(INSTALL) -m 644 $(LIB_DIR)/lib$(LIB).a '$(DESTDIR)$(LIBDIR)'
	$(INSTALL) -m 755 $(LIB_DIR)/lib$(DLL).la '$(DESTDIR)$(LIBDIR)'
	$(INSTALL) -m 755 $(LIB_DIR)/lib$(DLL).so.$(SOVER) '$(DESTDIR)$(LIBDIR)'
	ln -sf$(if $(VERBOSE),v) lib$(DLL).so.$(SOVER) '$(DESTDIR)$(LIBDIR)/lib$(DLL).so'
	$(INSTALL) -d '$(DESTDIR)$(PKG_CONFIG_DIR)'
	$(INSTALL) -m 644 $(LIB_DIR)/lib$(DLL).pc '$(DESTDIR)$(PKG_CONFIG_DIR)'
	$(LDCONFIG) -n$(if $(VERBOSE),v) '$(DESTDIR)$(LIBDIR)'

uninstall_libmemstack:
	rm -rf$(if $(VERBOSE),v) \
  '$(DESTDIR)$(PREFIX)/include/memstack' \
  '$(DESTDIR)$(LIBDIR)/lib$(LIB).a' \
  '$(DESTDIR)$(LIBDIR)/lib$(DLL).la' \
  '$(DESTDIR)$(LIBDIR)/lib$(DLL).so' \
  '$(DESTDIR)$(LIBDIR)/lib$(DLL).so.$(SOVER)' \
  '$(DESTDIR)$(PKG_CONFIG_DIR)/lib$(DLL).pc'
	$(LDCONFIG) -n$(if $(VERBOSE),v) '$(DESTDIR)$(LIBDIR)'

else ifeq (WINXX,$(OS))

install_libmemstack:
	$(if $(DESTDIR),,$(error DESTDIR - installation directory not defined))
	if not exist "$(DESTDIR)" mkdir "$(DESTDIR)"
	if not exist "$(DESTDIR)\include" mkdir "$(DESTDIR)\include"
	if not exist "$(DESTDIR)\include\memstack" mkdir "$(DESTDIR)\include\memstack"
	$(foreach f,$(HEADERS),copy /y $(call ospath,$(TOP)/memstack/$f) "$(DESTDIR)\include\memstack"$(newline))

endif # WINXX

.PHONY: install_libmemstack uninstall_libmemstack
