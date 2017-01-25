include $(dir $(lastword $(MAKEFILE_LIST)))../top.mk
include $(MTOP)/c.mk
include $(MTOP)/ctest.mk

EXE     := memstack_test_c
SRC     := memstack_test.c
INCLUDE := ..

ifdef DEBUG
DEFINES := MEMSTACK_DEBUG
LIBS    := memstackd
else
LIBS    := memstack
endif

$(DO_TEST_EXE)
$(call MAKE_CONTINUE,INCLUDE DEFINES LIBS)

EXE     := memstack_test_cpp
SRC     := memstack_test.cpp

$(DO_TEST_EXE)
$(DEFINE_TARGETS)
