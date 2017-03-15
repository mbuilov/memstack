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
TO_MAKE += test
endif

ifeq (LINUX,$(OS))

PREFIX         ?= /usr/local
EXEC_PREFIX    ?= $(PREFIX)
INCLUDEDIR     ?= $(PREFIX)/include
LIBDIR         ?= $(EXEC_PREFIX)/lib
PKG_CONFIG_DIR ?= $(LIBDIR)/pkgconfig
INSTALL        ?= install
LDCONFIG       ?= /sbin/ldconfig

else ifeq (WINXX,$(OS))

PREFIX     ?= dist
INCLUDEDIR ?= $(PREFIX)\include
LIBDIR     ?= $(PREFIX)\lib

endif # WINXX

install:
	@$(call ECHO,Successfully installed to $(DESTDIR)$(PREFIX))

uninstall:
	@$(call ECHO,Uninstalled from $(DESTDIR)$(PREFIX))

include $(MTOP)/parallel.mk

endif # !distclean
