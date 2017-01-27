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

$(DEFINE_TARGETS)

# installation

install_libmemstack: $(call FORM_TRG,LIB) $(call FORM_TRG,DLL)

install_libmemstack uninstall_libmemstack: LIB   := $(LIB)
install_libmemstack uninstall_libmemstack: DLL   := $(DLL)
install_libmemstack uninstall_libmemstack: SOVER := $(SOVER)

install_libmemstack: HEADERS := \
  memstack_base.h \
  memstack_comn.h \
  memstack_debug.h \
  memstack.h

ifeq (LINUX,$(OS))

include $(MTOP)/LINUX/pc.mk
include $(MTOP)/LINUX/la.mk

PC_DESC := Memory Stack allocation library

define PC_COMMENT
Author:  $(VENDOR_NAME)
License: LGPL version 2.1 or any later version
endef

install_libmemstack: PC_TEXT := $(call PKGCONFIG_DEF_TEMPLATE,$(DLL),$(SOVER),$(PC_DESC),$(PC_COMMENT),$(VENDOR_URL))
install_libmemstack: LA_TEXT := $(call LIBTOOL_LA_TEMPLATE,$(DLL),$(SOVER),$(LIB))

install_libmemstack:
	$(INSTALL) -d '$(DESTDIR)$(PREFIX)/include/memstack'
	$(INSTALL) -m 644 $(addprefix $(TOP)/memstack/,$(HEADERS)) '$(DESTDIR)$(PREFIX)/include/memstack'
	$(INSTALL) -d '$(DESTDIR)$(LIBDIR)'
	$(INSTALL) -m 644 $(LIB_DIR)/lib$(LIB).a '$(DESTDIR)$(LIBDIR)'
	$(INSTALL) -m 755 $(LIB_DIR)/lib$(DLL).so '$(DESTDIR)$(LIBDIR)/lib$(DLL).so.$(SOVER)'
	ln -sf$(if $(VERBOSE),v) lib$(DLL).so.$(SOVER) '$(DESTDIR)$(LIBDIR)/lib$(DLL).so'
	$(call ECHO,$(LA_TEXT)) > '$(DESTDIR)$(LIBDIR)/lib$(DLL).la'
	chmod$(if $(VERBOSE), -v) 755 '$(DESTDIR)$(LIBDIR)/lib$(DLL).la'
	$(INSTALL) -d '$(DESTDIR)$(PKG_CONFIG_DIR)'
	$(call ECHO,$(PC_TEXT)) > '$(DESTDIR)$(PKG_CONFIG_DIR)/lib$(DLL).pc'
	chmod$(if $(VERBOSE), -v) 644 '$(DESTDIR)$(PKG_CONFIG_DIR)/lib$(DLL).pc'
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

$(DESTDIR)$(PREFIX)/include/memstack: | $(DESTDIR)$(LIBDIR)
$(DESTDIR)$(PREFIX)/include/memstack $(DESTDIR)$(LIBDIR):
	$(call MKDIR,$@)

install_libmemstack: | $(DESTDIR)$(PREFIX)/include/memstack $(DESTDIR)$(LIBDIR)
	$(foreach f,$(HEADERS),$(call CP,$(TOP)/memstack/$f,"$(DESTDIR)$(PREFIX)/include/memstack")$(newline))
	$(call CP,$(LIB_DIR)/$(LIB)$(LIB_SUFFIX),"$(DESTDIR)$(LIBDIR)")
	$(call CP,$(IMP_DIR)/$(DLL)$(IMP_SUFFIX),"$(DESTDIR)$(LIBDIR)")
	$(call CP,$(DLL_DIR)/$(DLL)$(DLL_SUFFIX),"$(DESTDIR)$(LIBDIR)")

uninstall_libmemstack:
	$(call DEL_DIR,"$(DESTDIR)$(PREFIX)/include/memstack")
	$(call DEL,"$(DESTDIR)$(LIBDIR)/$(LIB)$(LIB_SUFFIX)")
	$(call DEL,"$(DESTDIR)$(LIBDIR)/$(DLL)$(IMP_SUFFIX)")
	$(call DEL,"$(DESTDIR)$(LIBDIR)/$(DLL)$(DLL_SUFFIX)")

endif # WINXX

.PHONY: install_libmemstack uninstall_libmemstack
