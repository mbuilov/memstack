include $(dir $(lastword $(MAKEFILE_LIST)))top.mk
include $(MTOP)/defs.mk

ifneq ($(filter distclean,$(MAKECMDGOALS)),)
distclean:
	$(call RM,$(CLOBBER_DIRS))
else

TO_MAKE := src

ifneq ($(filter WINXX,$(OS)),)
TO_MAKE += version
endif

ifneq ($(filter check tests clean,$(MAKECMDGOALS)),)
check tests: all
TO_MAKE += test
endif

ifeq (LINUX,$(OS))

PREFIX          ?= /usr/local
EXEC_PREFIX     ?= $(PREFIX)
LIBDIR          ?= $(EXEC_PREFIX)/lib
PKG_CONFIG_DIR  ?= $(LIBDIR)/pkgconfig
INSTALL         ?= install
LDCONFIG        ?= /sbin/ldconfig

else ifeq (WINXX,$(OS))

PREFIX ?= dist
LIBDIR ?= $(PREFIX)\lib

endif # WINXX

install: install_memstack
	@$(call ECHO,Successfully installed to $(DESTDIR)$(PREFIX))

uninstall: uninstall_memstack
	@$(call ECHO,Uninstalled from $(DESTDIR)$(PREFIX))

include $(MTOP)/parallel.mk

endif # !distclean
