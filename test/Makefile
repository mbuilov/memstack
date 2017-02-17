include $(dir $(lastword $(MAKEFILE_LIST)))../top.mk
include $(MTOP)/c.mk
include $(MTOP)/ctest.mk

EXE     := memstack_test_c
SRC     := memstack_test.c
INCLUDE := ..
RPATH   := $(LIB_DIR)

ifndef NO_STATIC
LIBS    := memstack
else ifndef NO_SHARED
DLLS    := memstack
else
$(error cannot link memstack library for tests)
endif

ifdef DEBUG
DEFINES := MEMSTACK_DEBUG
LIBS    := $(LIBS:=d)
DLLS    := $(DLLS:=d)
endif

SHLIBS := $(DLLS:=.$(call ver_major,$(PRODUCT_VER)))

$(call TEST_NEED_SIMLINKS,$(SHLIBS))

$(call DO_TEST_EXE,$(SHLIBS))

$(call MAKE_CONTINUE,INCLUDE DEFINES LIBS DLLS RPATH SHLIBS)

EXE     := memstack_test_cpp
SRC     := memstack_test.cpp

$(call DO_TEST_EXE,$(SHLIBS))

$(DEFINE_TARGETS)
