include $(dir $(lastword $(MAKEFILE_LIST)))project.mk
include $(MTOP)/defs.mk

ifeq (,$(filter distclean,$(MAKECMDGOALS)))

TO_MAKE := src

ifeq (WINXX,$(OS))
TO_MAKE += $(MTOP)/exts/version
endif

ifneq (,$(filter check tests clean,$(MAKECMDGOALS)))
TO_MAKE += test
endif

include $(MTOP)/parallel.mk

endif # !distclean
