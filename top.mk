# project root directory
TOP := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

# clean-build path
ifndef MTOP
$(error MTOP - path to clean-build (https://github.com/mbuilov/clean-build) is not defined,\
 example: MTOP=/usr/local/clean-build or MTOP=C:\User\clean-build)
endif
