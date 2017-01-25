include $(dir $(lastword $(MAKEFILE_LIST)))top.mk
include $(MTOP)/defs.mk

ifneq ($(filter distclean,$(MAKECMDGOALS)),)
distclean:
	$(call RM,$(CLOBBER_DIRS))
else

TO_MAKE := memstack.mk

ifneq ($(filter WINXX,$(OS)),)
TO_MAKE += version
endif

ifneq ($(filter check tests,$(MAKECMDGOALS)),)
check tests: all
TO_MAKE += test
endif

include $(MTOP)/parallel.mk

install: all
uninstall:

ifeq (LINUX,$(OS))

PREFIX          ?= /usr
EXEC_PREFIX     ?= $(PREFIX)
LIBDIR          ?= $(EXEC_PREFIX)/lib
PKG_CONFIG_DIR  ?= $(LIBDIR)/pkgconfig
INSTALL         ?= install
LDCONFIG        ?= /sbin/ldconfig

install: install_libmemstack
uninstall: uninstall_libmemstack

endif # LINUX

endif # !distclean
