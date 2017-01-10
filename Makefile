include $(dir $(lastword $(MAKEFILE_LIST)))top.mk
include $(MTOP)/defs.mk
ifneq ($(filter distclean,$(MAKECMDGOALS)),)
distclean:
	$(call RM,$(CLOBBER_DIRS) $(TOP)/config.mk)
else
install: all
include $(TOP)/make/$(OS)/install.mk
TO_MAKE := version memstack.mk
include $(MTOP)/parallel.mk
endif
