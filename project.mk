# the only case when TOP is defined - after completing project configuration
ifneq (override,$(origin TOP))

# project configuration file

# TOP - project root directory
# define this variable for referencing project files: sources, makefiles, include paths, etc.
# Note: TOP variable is not used by clean-build
override TOP := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

# specify version of clean-build build system required by this project
CLEAN_BUILD_REQUIRED_VERSION := 0.6.3

# BUILD - variable required by clean-build - path to built artifacts
BUILD := $(TOP)/build

# global product version
# Note: this is default value of MODVER - per-module version number
# format: major.minor.patch
PRODUCT_VER := 1.0.3

# next variables are needed for generating:
# - header file with version info (see $(MTOP)/exts/version/version.mk)
# - under Windows, resource files with version info (see $(MTOP)/WINXX/cres.mk)
PRODUCT_NAMES_H  := vers.h
VENDOR_NAME      := Michael M. Builov
PRODUCT_NAME     := Memory Stack allocation library
VENDOR_URL       := https://github.com/mbuilov/memstack
VENDOR_COPYRIGHT := Copyright (C) 2008-2017 $(VENDOR_NAME), $(VENDOR_URL)

# licence for generated pkgconfig .pc file
PRODUCT_LICENCE  := LGPL version 2.1 or any later version

# default target
TARGET := RELEASE

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

# optional, clean-build generated config file (while completing 'conf' goal)
#
# Note: generated $(CONFIG) file will remember values of command-line or overridden variables;
#  by sourcing $(CONFIG) file, these variables are will be restored
#  and only new command-line values may override restored variables
#
# Note: by completing 'distclean' goal, defined by clean-build, $(BUILD) directory will be deleted
#  - together with $(CONFIG) file, if it was generated under $(BUILD)
#
# Note: define CONFIG as recursive variable
#  - for the case when BUILD is redefined in next included $(OVERRIDES) makefile
CONFIG = $(BUILD)/conf.mk

# adjust project defaults
# OVERRIDES may be specified in command line, which overrides next empty definition
OVERRIDES:=
ifdef OVERRIDES
ifeq (,$(wildcard $(OVERRIDES)))
$(error cannot include $(OVERRIDES))
endif
include $(OVERRIDES)
endif

# source clean-build generated config file, if it exist
-include $(CONFIG)

# clean-build path must be defined, to include clean-build definitions
ifndef MTOP
$(error MTOP - path to clean-build (https://github.com/mbuilov/clean-build) is not defined,\
 example: MTOP=/usr/local/clean-build or MTOP=C:\User\clean-build)
endif

endif # TOP
