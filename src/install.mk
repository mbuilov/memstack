# this file included by $(TOP)/src/Makefile

# define
# NO_INSTALL_HEADERS - to not install/uninstall header files
# NO_INSTALL_LA      - to not install/uninstall libtool .la-files (UNIX)
# NO_INSTALL_PC      - to not install/uninstall pkg-config .pc-files (UNIX)
# NO_INSTALL_IMPS    - to not install/uninstall dll implementation libraries (WINDOWS)

# also, if defined
# NO_STATIC          - static libraries are not installed/uninstalled
# NO_SHARED          - dynamic libraries are not installed/uninstalled

BUILT_LIB_VARIANTS := $(if $(LIB),$(call GET_VARIANTS,LIB))
BUILT_DLL_VARIANTS := $(if $(DLL),$(call GET_VARIANTS,DLL))
BUILT_LIBS         := $(foreach v,$(BUILT_LIB_VARIANTS),$(call FORM_TRG,LIB,$v))
BUILT_DLLS         := $(foreach v,$(BUILT_DLL_VARIANTS),$(call FORM_TRG,DLL,$v))

install_libmemstack: $(BUILT_LIBS) $(BUILT_DLLS)

install_libmemstack uninstall_libmemstack: BUILT_LIBS := $(BUILT_LIBS)
install_libmemstack uninstall_libmemstack: BUILT_DLLS := $(BUILT_DLLS)

install_libmemstack_headers: HEADERS := \
  memstack_base.h \
  memstack_comn.h \
  memstack.h

ifdef DEBUG
install_libmemstack_headers: HEADERS += memstack_debug.h
endif

ifeq (LINUX,$(OS))

include $(MTOP)/UNIX/libs.mk
include $(MTOP)/UNIX/pc.mk
include $(MTOP)/UNIX/la.mk

install_libmemstack uninstall_libmemstack: MODVER   := $(MODVER)
install_libmemstack uninstall_libmemstack: DEFINES  := $(DEFINES)
install_libmemstack uninstall_libmemstack: ALL_LIBS := $(call \
  GET_ALL_LIBS,$(BUILT_LIBS),$(BUILT_LIB_VARIANTS),$(BUILT_DLLS),$(BUILT_DLL_VARIANTS))

define PC_COMMENT
Author:  $(VENDOR_NAME)
License: $(PRODUCT_LICENCE)
endef

# choose CFLAGS option for static library variant $1
VARIANT_CFLAGS = $(if \
  $(filter P,$1), $(PIE_OPTION),$(if \
  $(filter D,$1), $(PIC_OPTION)))

# generate contents of .pc-file
# $1 - static or dynamic library name
# $2 - library variant
PC_GENERATOR = $(call PKGCONF_DEF_TEMPLATE,$1,$(MODVER),$(PRODUCT_NAME),$(PC_COMMENT),$(VENDOR_URL),,,,$(addprefix \
  -D,$(DEFINES))$(call VARIANT_CFLAGS,$2))

install_libmemstack_headers:
	$(INSTALL) -d '$(DESTDIR)$(PREFIX)/include/memstack'
	$(INSTALL) -m 644 $(addprefix $(TOP)/memstack/,$(HEADERS)) '$(DESTDIR)$(PREFIX)/include/memstack'

install_libmemstack: $(if $(NO_INSTALL_HEADERS),,install_libmemstack_headers)
	$(if $(BUILT_LIBS)$(BUILT_DLLS),$(INSTALL) -d '$(DESTDIR)$(LIBDIR)')
	$(foreach l,$(BUILT_LIBS),$(newline)$(INSTALL) -m 644 $l '$(DESTDIR)$(LIBDIR)')
	$(foreach d,$(BUILT_DLLS),$(newline)$(INSTALL) -m 755 $d '$(DESTDIR)$(LIBDIR)/$(notdir $d).$(MODVER)')
	$(foreach d,$(BUILT_DLLS),$(newline)ln -sf$(if $(VERBOSE),v) $(notdir $d).$(MODVER) '$(DESTDIR)$(LIBDIR)/$(notdir $d)')
	$(if $(NO_INSTALL_LA),,$(call INSTALL_LAS,$(ALL_LIBS),$(BUILT_LIBS),$(BUILT_DLLS)))
	$(if $(NO_INSTALL_PC),,$(if $(BUILT_LIBS)$(BUILT_DLLS),$(INSTALL) -d '$(DESTDIR)$(PKG_CONFIG_DIR)'))
	$(if $(NO_INSTALL_PC),,$(call INSTALL_PKGCONFS,$(ALL_LIBS),PC_GENERATOR))
	$(if $(BUILT_DLLS),$(LDCONFIG) -n$(if $(VERBOSE),v) '$(DESTDIR)$(LIBDIR)')

uninstall_libmemstack:
	rm -rf$(if $(VERBOSE),v) $(if \
  $(NO_INSTALL_HEADERS),,'$(DESTDIR)$(PREFIX)/include/memstack') $(foreach \
  l,$(BUILT_LIBS),'$(DESTDIR)$(LIBDIR)/$(notdir $l)') $(foreach \
  d,$(BUILT_DLLS),'$(DESTDIR)$(LIBDIR)/$(notdir $d)' '$(DESTDIR)$(LIBDIR)/$(notdir $d).$(MODVER)') $(if \
  $(NO_INSTALL_LA),,$(call INSTALLED_LAS,$(ALL_LIBS),$(BUILT_LIBS),$(BUILT_DLLS))) $(if \
  $(NO_INSTALL_PC),,$(call INSTALLED_PKGCONFS,$(ALL_LIBS)))
	$(if $(BUILT_DLLS),$(LDCONFIG) -n$(if $(VERBOSE),v) '$(DESTDIR)$(LIBDIR)')

else ifeq (WINXX,$(OS))

install_libmemstack uninstall_libmemstack: BUILT_IMPS := $(foreach \
  v,$(BUILT_DLL_VARIANTS),$(call MAKE_IMP_PATH,$(call FORM_TRG,DLL,$v),$v))

DST_INC_DIR := $(subst $(space),\$(space),$(DESTDIR)$(PREFIX)/include/memstack)
DST_LIB_DIR := $(subst $(space),\$(space),$(DESTDIR)$(LIBDIR))

$(DST_LIB_DIR): | $(if $(NO_INSTALL_HEADERS),,$(DST_INC_DIR))
$(DST_INC_DIR) $(DST_LIB_DIR):
	$(call MKDIR,"$(subst \ , ,$@)")

install_libmemstack_headers: | $(DST_INC_DIR)
	$(foreach f,$(HEADERS),$(call CP,$(TOP)/memstack/$f,"$(DESTDIR)$(PREFIX)/include/memstack")$(newline))

install_libmemstack: $(if $(NO_INSTALL_HEADERS),,install_libmemstack_headers) | $(DST_LIB_DIR)
	$(foreach l,$(BUILT_LIBS),$(newline)$(call CP,$l,"$(DESTDIR)$(LIBDIR)"))
	$(foreach d,$(BUILT_DLLS),$(newline)$(call CP,$d,"$(DESTDIR)$(LIBDIR)"))
	$(if $(NO_INSTALL_IMPS),,$(foreach i,$(BUILT_IMPS),$(newline)$(call CP,$i,"$(DESTDIR)$(LIBDIR)")))

uninstall_libmemstack:
	$(if $(NO_INSTALL_HEADERS),,$(call DEL_DIR,"$(DESTDIR)$(PREFIX)/include/memstack"))
	$(foreach l,$(notdir $(BUILT_LIBS)),$(newline)$(call DEL,"$(DESTDIR)$(LIBDIR)/$l"))
	$(foreach d,$(notdir $(BUILT_DLLS)),$(newline)$(call DEL,"$(DESTDIR)$(LIBDIR)/$d"))
	$(if $(NO_INSTALL_IMPS),,$(foreach i,$(notdir $(BUILT_IMPS)),$(newline)$(call DEL,"$(DESTDIR)$(LIBDIR)/$i")))

endif # WINXX

.PHONY: install_libmemstack_headers install_libmemstack uninstall_libmemstack
