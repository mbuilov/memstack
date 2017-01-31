include $(dir $(lastword $(MAKEFILE_LIST)))../top.mk
include $(MTOP)/c.mk
include $(MTOP)/ctest.mk

EXE     := memstack_test_c
SRC     := memstack_test.c
INCLUDE := ..

ifndef NO_STATIC
LIBS    := memstack
else ifndef NO_SHARED
DLLS    := memstack
else
$(error cannot link memstack library for tests)
endif

ifdef DEBUG
DEFINES := MEMSTACK_DEBUG
LIBS    := $(addsuffix d,$(LIBS))
DLLS    := $(addsuffix d,$(DLLS))
endif

$(DO_TEST_EXE)
$(call MAKE_CONTINUE,INCLUDE DEFINES LIBS DLLS)

EXE     := memstack_test_cpp
SRC     := memstack_test.cpp

$(DO_TEST_EXE)
$(DEFINE_TARGETS)
