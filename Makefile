include $(dir $(lastword $(MAKEFILE_LIST)))make/project.mk
include $(MTOP)/defs.mk

ifeq (,$(filter distclean,$(MAKECMDGOALS)))

TO_MAKE := src

ifdef OS_WINXX
TO_MAKE += version
endif

ifneq (,$(filter check tests clean,$(MAKECMDGOALS)))
TO_MAKE += test
endif

include $(MTOP)/parallel.mk

endif # !distclean
