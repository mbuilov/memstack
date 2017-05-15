include $(dir $(lastword $(MAKEFILE_LIST)))../project.mk
include $(MTOP)/c.mk
include $(MTOP)/exts/ctest.mk

INCLUDE := ..
RPATH   := $(LIB_DIR)

# don't test D-variant of static memstack library - this variant is for linking static library to some dll
TEST_LIB_VARIANTS := $(addsuffix -lib,$(if $(NO_STATIC),,$(filter-out D,$(call FILTER_VARIANTS_LIST,LIB,$(MEMSTACK_LIB_VARIANTS)))))
TEST_DLL_VARIANTS := $(addsuffix -dll,$(if $(NO_SHARED),,$(call FILTER_VARIANTS_LIST,DLL,$(MEMSTACK_DLL_VARIANTS))))

# form test executable name
# $1 - dyn or <empty>
# $v - variant
# memstack_test.c -> memstack_test_c-dyn-s
FORM_EXE_NAME = $(subst .,_,$(SRC))$(addprefix -,$1)$(addprefix -,$(call tolower,$(filter-out R,$v))) $v

# $v - static library variant to test
define TEST_LIB_TEMPLATE
LIBS    := $(MEMSTACK_LIB_NAME)
EXE     := $(FORM_EXE_NAME)
DEFINES := $(if $(DEBUG),MEMSTACK_DEBUG)
$$(DO_TEST_EXE)
endef

# $v - dynamic library variant to test
# note: add $(LIB_DIR) to $(PATH) to run tests under cygwin
define TEST_DLL_TEMPLATE
DLLS    := $(MEMSTACK_LIB_NAME)
EXE     := $(call FORM_EXE_NAME,dyn)
DEFINES := $(if $(DEBUG),MEMSTACK_DEBUG) MEMSTACK_EXPORTS=$(DLL_IMPORTS_DEFINE)
$$(call DO_TEST_EXE,$$(DLLS:=.$(call ver_major,$(PRODUCT_VER))),,,PATH=$$$$(PATH)$$(PATHSEP)$$(LIB_DIR))
endef

# expand $(TEST_LIB_TEMPLATE) or $(TEST_DLL_TEMPLATE)
# $x - r-lib, r-dll, s-lib, ...
EXPAND_TEST_TEMPLATE = $(foreach v,$(firstword $(subst -, ,$x)),$(if $(filter %-lib,$x),$(TEST_LIB_TEMPLATE),$(TEST_DLL_TEMPLATE)))

SRC := memstack_test.c

$(foreach x,$(TEST_LIB_VARIANTS) $(TEST_DLL_VARIANTS),$(eval \
  $(EXPAND_TEST_TEMPLATE))$(call MAKE_CONTINUE,SRC INCLUDE RPATH))

SRC := memstack_test.cpp

$(foreach x,$(TEST_LIB_VARIANTS) $(TEST_DLL_VARIANTS),$(eval \
  $(EXPAND_TEST_TEMPLATE))$(call MAKE_CONTINUE,SRC INCLUDE RPATH))

$(DEFINE_TARGETS)
