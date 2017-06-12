include $(dir $(lastword $(MAKEFILE_LIST)))../project.mk
include $(MTOP)/parallel.mk
$(call PROCESS_SUBMAKES,memstack.mk $(if $(filter install uninstall,$(MAKECMDGOALS)),install.mk))
