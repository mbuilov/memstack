# the only case when TOP is defined - after completing project configuration
ifneq (override,$(origin TOP))

# project configuration file

# TOP - project root directory
# define this variable for referencing project files: sources, makefiles, include paths, etc.
# note: TOP variable is not used by clean-build
override TOP := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

# specify version of clean-build build system required by this project
CLEAN_BUILD_REQUIRED_VERSION := 0.6.3

# BUILD - variable required by clean-build - path to built artifacts
BUILD := $(TOP)/build

# optional, clean-build generated config file (while completing 'conf' goal)
# note: define CONFIG_FILE as recursive variable - for the case when BUILD is defined in command line as recursive
# note: clean-build will override CONFIG_FILE to make it non-recursive (simple)
CONFIG_FILE = $(BUILD)/conf.mk

# default value for MODVER
# major.minor.patch
PRODUCT_VER := 1.0.3

# header generated by $(MTOP)/exts/version/version.mk
# $(PRODUCT_NAMES_H) - used in $(MTOP)/WINXX/cres.mk
PRODUCT_NAMES_H := vers.h

# version info for a dll, also needed by $(MTOP)/exts/version/version.mk
VENDOR_NAME      := Michael M. Builov
VENDOR_URL       := https://github.com/mbuilov/memstack
PRODUCT_NAME     := Memory Stack allocation library
VENDOR_COPYRIGHT := Copyright (C) 2008-2017 $(VENDOR_NAME), $(VENDOR_URL)

# licence for generated pkgconfig .pc file
PRODUCT_LICENCE  := LGPL version 2.1 or any later version

# library name
MEMSTACK_LIB_NAME := memstack$(if $(filter DEBUG,$(TARGET)),d)

# variants of built static library
#  LINUX:
#   R,S   - position-dependent code
#   P     - position-independent code for executables
#   D     - position-independent code for shared objects (dlls)
#  WINDOWS:
#   R,P,D - dynamically linked multi-threaded libc
#   S     - statically linked multi-threaded libc
MEMSTACK_LIB_VARIANTS := R P D S

# variants of built dynamic library
#  LINUX:
#   R,S   - position-independent code
#  WINDOWS:
#   R     - dynamically linked multi-threaded libc
#   S     - statically linked multi-threaded libc
MEMSTACK_DLL_VARIANTS := R S

# by default, build and install both shared and static variants of the library
NO_SHARED:=
NO_STATIC:=

# define _DEBUG in debugging build
# note: redefine PREDEFINES variable as non-recursive (simple) on first call
PREDEFINES = $(call lazy_simple,PREDEFINES,$(if $(DEBUG),_DEBUG) $(OS_PREDEFINES))

# adjust project defaults, add missing definitions
ifeq ("command line","$(origin PROJECT_OVERRIDES)")
ifeq (,$(wildcard $(PROJECT_OVERRIDES)))
$(error cannot include $(PROJECT_OVERRIDES))
endif
include $(PROJECT_OVERRIDES)
endif

# source config, if it was previously generated by clean-build
-include $(CONFIG_FILE)

# clean-build path must be defined, to include clean-build definitions
ifndef MTOP
$(error MTOP - path to clean-build (https://github.com/mbuilov/clean-build) is not defined,\
 example: MTOP=/usr/local/clean-build or MTOP=C:\User\clean-build)
endif

endif # TOP
