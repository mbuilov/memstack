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
LIBS    := $(addsuffix d,$(LIBS))
DLLS    := $(addsuffix d,$(DLLS))
endif

ifeq (UNIX,$(OSTYPE))
ifdef DLLS
DLINK := $(LIB_DIR)/lib$(DLLS).so.$(PRODUCT_VERSION_MAJOR)
$(call ADD_GENERATED,$(DLINK))
$(DLINK): $(LIB_DIR)/lib$(DLLS).so
	$(call SUP,LN,$@)ln -sf$(if $(VERBOSE),v) $(notdir $<) $@
endif
endif

$(BIN_DIR)/$(EXE): | $(DLINK)

$(DO_TEST_EXE)
$(call MAKE_CONTINUE,INCLUDE DEFINES LIBS DLLS RPATH)

EXE     := memstack_test_cpp
SRC     := memstack_test.cpp

$(BIN_DIR)/$(EXE): | $(DLINK)

$(DO_TEST_EXE)
$(DEFINE_TARGETS)
