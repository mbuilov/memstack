include $(dir $(lastword $(MAKEFILE_LIST)))../top.mk
include $(MTOP)/c.mk
include $(MTOP)/ctest.mk

ifdef NO_STATIC
ifdef NO_SHARED
$(error cannot link $(MEMSTACK_LIB_NAME) library for tests)
endif
endif

INCLUDE := ..
DEFINES := $(if $(DEBUG),MEMSTACK_DEBUG)
RPATH   := $(LIB_DIR)

# don't test D-variant of static library - this variant for linking static library to dll
TEST_LIB_VARIANTS := $(addsuffix -lib,$(if $(NO_STATIC),,$(filter-out D,$(call FILTER_VARIANTS_LIST,LIB,$(MEMSTACK_LIB_VARIANTS)))))
TEST_DLL_VARIANTS := $(addsuffix -dll,$(if $(NO_SHARED),,$(call FILTER_VARIANTS_LIST,DLL,$(MEMSTACK_DLL_VARIANTS))))

# form test executable name
# $1 - dyn or <empty>
# $v - variant
FORM_EXE_NAME = $(subst .,_,$(SRC))$(addprefix -,$1)$(addprefix -,$(call tolower,$(filter-out R,$v))) $v

# $v - static library variant to test
define TEST_LIB_TEMPLATE
LIBS    := $(MEMSTACK_LIB_NAME)
EXE     := $(call FORM_EXE_NAME)
$$(DO_TEST_EXE)
endef

# $1 - if non-empty, then generate rules for simlinks
# $v - dynamic library variant to test
define TEST_DLL_TEMPLATE
DLLS    := $(MEMSTACK_LIB_NAME)$(call DLL_VAR_SUFFIX,DLL,$v,$(MEMSTACK_DLL_VARIANTS))
EXE     := $(call FORM_EXE_NAME,dyn)
SHLIBS  := $$(DLLS:=.$(call ver_major,$(PRODUCT_VER)))
$(if $1,$$(call TEST_NEED_SIMLINKS,$$(SHLIBS)))
$$(call DO_TEST_EXE,$$(SHLIBS))
endef

# expand $(TEST_LIB_TEMPLATE) or $(TEST_DLL_TEMPLATE)
# $1 - 1 or <empty>
# $x - r-lib, r-dll, s-lib, ...
EXPAND_TEST_TEMPLATE = $(foreach v,$(firstword $(subst -, ,$x)),$(if $(filter %-lib,$x),$(TEST_LIB_TEMPLATE),$(TEST_DLL_TEMPLATE)))

SRC := memstack_test.c

$(foreach x,$(TEST_LIB_VARIANTS) $(TEST_DLL_VARIANTS),$(eval \
  $(call EXPAND_TEST_TEMPLATE,1))$(call MAKE_CONTINUE,SRC INCLUDE DEFINES RPATH))

SRC := memstack_test.cpp

$(foreach x,$(wordlist 2,999999,$(TEST_LIB_VARIANTS) $(TEST_DLL_VARIANTS)),$(eval \
  $(EXPAND_TEST_TEMPLATE))$(call MAKE_CONTINUE,SRC INCLUDE DEFINES RPATH))

$(foreach x,$(firstword $(TEST_LIB_VARIANTS) $(TEST_DLL_VARIANTS)),$(eval $(EXPAND_TEST_TEMPLATE)))

# to run tests under cygwin
check: PATH := $(PATH):$(LIB_DIR)

$(DEFINE_TARGETS)
