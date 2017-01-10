# project root directory
TOP := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

# include config, if it was generated
-include $(TOP)/config.mk

# clean-build path
ifndef MTOP
$(error MTOP - path to clean-build is not defined, example: MTOP=/usr/local/clean-build or MTOP=C:\User\clean-build)
endif
