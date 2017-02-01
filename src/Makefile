include $(dir $(lastword $(MAKEFILE_LIST)))../top.mk
include $(MTOP)/c.mk

LIB     := $(if $(NO_STATIC),,memstack)
DLL     := $(if $(NO_SHARED),,memstack)
SRC     := memstack.c
INCLUDE := ..
SOVER   := $(PRODUCT_VERSION_MAJOR).$(PRODUCT_VERSION_MINOR).$(PRODUCT_PATCH_NUM)

DLL_DEFINES := MEMSTACK_EXPORTS=$(DLL_EXPORTS_DEFINE)

ifdef DEBUG
LIB     := $(LIB:=d)
DLL     := $(DLL:=d)
SRC     += memstack_debug.c
DEFINES := MEMSTACK_DEBUG
endif

$(DEFINE_TARGETS)

# installation

install_libmemstack_check_libs:
	$(if $(DLL)$(LIB),,$(error no libs to install))

install_libmemstack: install_libmemstack_check_libs $(call FORM_TRG,LIB) $(call FORM_TRG,DLL)

install_libmemstack uninstall_libmemstack: LIB   := $(LIB)
install_libmemstack uninstall_libmemstack: DLL   := $(DLL)
install_libmemstack uninstall_libmemstack: SOVER := $(SOVER)
install_libmemstack uninstall_libmemstack: LIB_NAME := $(firstword $(DLL) $(LIB))

install_libmemstack_headers: HEADERS := \
  memstack_base.h \
  memstack_comn.h \
  memstack.h

ifdef DEBUG
install_libmemstack_headers: HEADERS += memstack_debug.h
endif

ifeq (LINUX,$(OS))

include $(MTOP)/LINUX/pc.mk
include $(MTOP)/LINUX/la.mk

PC_DESC := Memory Stack allocation library

define PC_COMMENT
Author:  $(VENDOR_NAME)
License: LGPL version 2.1 or any later version
endef

install_libmemstack: LA_TEXT := $(call LIBTOOL_LA_TEMPLATE,$(DLL),$(SOVER),$(LIB))
install_libmemstack: PC_TEXT := $(call \
  PKGCONFIG_DEF_TEMPLATE,$(LIB_NAME),$(SOVER),$(PC_DESC),$(PC_COMMENT),$(VENDOR_URL),,,,$(addprefix -D,$(DEFINES)))

install_libmemstack_headers:
	$(INSTALL) -d '$(DESTDIR)$(PREFIX)/include/memstack'
	$(INSTALL) -m 644 $(addprefix $(TOP)/memstack/,$(HEADERS)) '$(DESTDIR)$(PREFIX)/include/memstack'

install_libmemstack: install_libmemstack_headers
	$(INSTALL) -d '$(DESTDIR)$(LIBDIR)'
	$(if $(LIB),$(INSTALL) -m 644 $(LIB_DIR)/lib$(LIB).a '$(DESTDIR)$(LIBDIR)')
	$(if $(DLL),$(INSTALL) -m 755 $(LIB_DIR)/lib$(DLL).so '$(DESTDIR)$(LIBDIR)/lib$(DLL).so.$(SOVER)')
	$(if $(DLL),ln -sf$(if $(VERBOSE),v) lib$(DLL).so.$(SOVER) '$(DESTDIR)$(LIBDIR)/lib$(DLL).so')
	$(call ECHO,$(LA_TEXT)) > '$(DESTDIR)$(LIBDIR)/lib$(LIB_NAME).la'
	chmod$(if $(VERBOSE), -v) 755 '$(DESTDIR)$(LIBDIR)/lib$(LIB_NAME).la'
	$(INSTALL) -d '$(DESTDIR)$(PKG_CONFIG_DIR)'
	$(call ECHO,$(PC_TEXT)) > '$(DESTDIR)$(PKG_CONFIG_DIR)/lib$(LIB_NAME).pc'
	chmod$(if $(VERBOSE), -v) 644 '$(DESTDIR)$(PKG_CONFIG_DIR)/lib$(LIB_NAME).pc'
	$(LDCONFIG) -n$(if $(VERBOSE),v) '$(DESTDIR)$(LIBDIR)'

uninstall_libmemstack:
	rm -rf$(if $(VERBOSE),v) \
  '$(DESTDIR)$(PREFIX)/include/memstack' \
  $(if $(LIB),'$(DESTDIR)$(LIBDIR)/lib$(LIB).a') \
  $(if $(DLL),'$(DESTDIR)$(LIBDIR)/lib$(DLL).so') \
  $(if $(DLL),'$(DESTDIR)$(LIBDIR)/lib$(DLL).so.$(SOVER)') \
  '$(DESTDIR)$(LIBDIR)/lib$(LIB_NAME).la' \
  '$(DESTDIR)$(PKG_CONFIG_DIR)/lib$(LIB_NAME).pc'
	$(LDCONFIG) -n$(if $(VERBOSE),v) '$(DESTDIR)$(LIBDIR)'

else ifeq (WINXX,$(OS))

DST_INC_DIR := $(subst $(space),\$(space),$(DESTDIR)$(PREFIX)/include/memstack)
DST_LIB_DIR := $(subst $(space),\$(space),$(DESTDIR)$(LIBDIR))

ifneq ($(DLL)$(LIB),)
$(DST_INC_DIR): | $(DST_LIB_DIR)
endif

$(DST_INC_DIR) $(DST_LIB_DIR):
	$(call MKDIR,"$(subst \ , ,$@)")

install_libmemstack_headers: | $(DST_INC_DIR)
	$(foreach f,$(HEADERS),$(call CP,$(TOP)/memstack/$f,"$(DESTDIR)$(PREFIX)/include/memstack")$(newline))

install_libmemstack: install_libmemstack_headers | $(DST_LIB_DIR)
	$(if $(LIB),$(call CP,$(LIB_DIR)/$(LIB)$(LIB_SUFFIX),"$(DESTDIR)$(LIBDIR)"))
	$(if $(DLL).$(call CP,$(IMP_DIR)/$(DLL)$(IMP_SUFFIX),"$(DESTDIR)$(LIBDIR)"))
	$(if $(DLL),$(call CP,$(DLL_DIR)/$(DLL)$(DLL_SUFFIX),"$(DESTDIR)$(LIBDIR)"))

uninstall_libmemstack:
	$(call DEL_DIR,"$(DESTDIR)$(PREFIX)/include/memstack")
	$(if $(LIB).$(call DEL,"$(DESTDIR)$(LIBDIR)/$(LIB)$(LIB_SUFFIX)"))
	$(if $(DLL),$(call DEL,"$(DESTDIR)$(LIBDIR)/$(DLL)$(IMP_SUFFIX)"))
	$(if $(DLL),$(call DEL,"$(DESTDIR)$(LIBDIR)/$(DLL)$(DLL_SUFFIX)"))

endif # WINXX

.PHONY: install_libmemstack_check_libs install_libmemstack_headers install_libmemstack uninstall_libmemstack
