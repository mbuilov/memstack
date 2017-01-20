include $(dir $(lastword $(MAKEFILE_LIST)))top.mk
include $(MTOP)/defs.mk
ifneq ($(filter distclean,$(MAKECMDGOALS)),)
distclean:
	$(call RM,$(CLOBBER_DIRS))
else
check: all
	$(MAKE) -C test
	$(BIN_DIR)/memstack_test
install: all
uninstall:
TO_MAKE := version memstack.mk
include $(MTOP)/parallel.mk
endif
