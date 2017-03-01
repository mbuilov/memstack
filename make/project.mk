# this file is included by $(MTOP)/defs.mk

SUPPORTED_OSES    := WINXX SOLARIS LINUX
SUPPORTED_CPUS    := x86 x86_64 sparc sparc64 armv5 mips24k ppc
SUPPORTED_TARGETS := MEMSTACK MEMSTACKD

DEFINCLUDE :=

PREDEFINES := $(if $(DEBUG),_DEBUG) TARGET_$(patsubst %D,%,$(TARGET)) \
              $(if $(filter sparc% mips% ppc%,$(CPU)),B_ENDIAN BIG_ENDIAN,L_ENDIAN) \
              $(if $(filter sparc% mips% ppc% arm%,$(CPU)),ADDRESS_NEEDALIGN)

APPDEFS :=

ifeq (WINXX,$(OS))
PREDEFINES += _ALLOW_KEYWORD_MACROS _ALLOW_RTCc_IN_STL inline=__inline
WIN_APP_FLAGS := /X /GF /Wall /EHsc
ifdef CXXCC
WIN_APP_FLAGS += /TP
endif
ifdef DEBUG
WIN_APP_FLAGS += /Od /Zi /RTCc /RTCsu /GS
else
WIN_APP_FLAGS += /Ox /GL /Gy
endif
WIN_APP_FLAGS += /wd4996# 'strdup': The POSIX name for this item is deprecated...
WIN_APP_FLAGS += /wd4820# 'x' bytes padding added after data member 'y'
WIN_APP_FLAGS += /wd4514# 'function' : unreferenced inline function has been removed
WIN_APP_FLAGS += /wd4710# function not inlined
WIN_APP_FLAGS += /wd4711# function selected for automatic inline expansion
WIN_APP_FLAGS += /wd4738# storing 32-bit float result in memory, possible loss of performance
WIN_APP_FLAGS += /wd4571# Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
ifndef APP_FLAGS
APP_FLAGS := $(WIN_APP_FLAGS)
endif
ifndef WDK
DDK:=none# not building drivers
endif
endif

ifeq (LINUX,$(OS))
ifndef APP_FLAGS
  APP_FLAGS :=
  DEF_SHARED_LIBS :=
  DEF_EXE_FLAGS :=
  DEF_SO_FLAGS := -shared -Wl,--no-undefined
  ifdef DEBUG
    APP_FLAGS += -fno-common -fno-omit-frame-pointer
    ifndef NO_STACK_PROTECTOR
      APP_FLAGS     += -fstack-protector-all
      DEF_EXE_FLAGS += -fstack-protector-all
      DEF_SO_FLAGS  += -fstack-protector-all
    endif # NO_STACK_PROTECTOR
    APP_FLAGS += $(if $(filter GCC%,$(COMPILER_TYPE)),-ggdb3,-g)
    SANITIZE :=
    ifndef NO_SANITIZE
      ifneq ($(filter GCC5 GCC6,$(COMPILER_TYPE)),)
        SANITIZE += -fsanitize=undefined
        SANITIZE += -fsanitize=address
        ifeq ($(COMPILER_TYPE),GCC6)
          SANITIZE += -fsanitize=bounds-strict
        endif
        DEF_SHARED_FLAGS := -Wl,--warn-common -Wl,--no-demangle $(SANITIZE)
      else ifneq ($(filter CLANG,$(COMPILER_TYPE)),)
        SANITIZE += -fsanitize=address
        SANITIZE += -fsanitize=undefined
        SANITIZE += -fsanitize=integer
        DEF_SO_FLAGS := $(filter-out %--no-undefined,$(DEF_SO_FLAGS))
        DEF_EXE_FLAGS += $(SANITIZE)
      endif # CLANG
    endif # NO_SANITIZE
    COVERAGE :=
    ifndef NO_COVERAGE
      ifneq ($(filter GCC5 GCC6 CLANG,$(COMPILER_TYPE)),)
        COVERAGE := --coverage
      endif
    endif # NO_COVERAGE
    ifneq ($(filter GCC5 GCC6,$(COMPILER_TYPE)),)
      DEF_SHARED_FLAGS := -Wl,--warn-common -Wl,--no-demangle $(SANITIZE) $(COVERAGE)
    else ifneq ($(filter CLANG,$(COMPILER_TYPE)),)
      DEF_SO_FLAGS := $(filter-out %--no-undefined,$(DEF_SO_FLAGS))
      DEF_EXE_FLAGS += $(COVERAGE)
    endif # CLANG
    APP_FLAGS += $(SANITIZE) $(COVERAGE)
  else # !DEBUG
    APP_FLAGS := -g -O2
  endif # !DEBUG
endif # !APP_FLAGS
GCC4_VERB_CXXFLAGS := -fstrict-overflow -Waddress -Waggressive-loop-optimizations -Wall -Wcast-align -Wcast-qual -Wchar-subscripts -Wclobbered -Wcomment -Wconversion -Wcoverage-mismatch -Wdisabled-optimization -Wdouble-promotion -Wempty-body -Wenum-compare -Wextra -Wfloat-equal -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wignored-qualifiers -Winit-self -Winline -Winvalid-pch -Wlogical-op -Wmain -Wmaybe-uninitialized -Wmissing-braces -Wmissing-declarations -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wnarrowing -Wnonnull -Woverlength-strings -Wpacked -Wpacked-bitfield-compat -Wparentheses -Wpedantic -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wshadow -Wsign-compare -Wsign-conversion -Wsizeof-pointer-memaccess -Wstack-protector -Wstrict-aliasing=1 -Wstrict-overflow=5 -Wsuggest-attribute=const -Wsuggest-attribute=format -Wsuggest-attribute=noreturn -Wsuggest-attribute=pure -Wswitch -Wswitch-enum -Wsync-nand -Wtrampolines -Wtrigraphs -Wtype-limits -Wundef -Wuninitialized -Wunknown-pragmas -Wunsafe-loop-optimizations -Wunused -Wunused-but-set-parameter -Wunused-but-set-variable -Wunused-function -Wunused-label -Wunused-local-typedefs -Wunused-parameter -Wunused-value -Wunused-variable -Wvector-operation-performance -Wvla -Wvolatile-register-var -Wwrite-strings -Wno-old-style-cast -Wno-long-long -Wno-variadic-macros -fvisibility-inlines-hidden
GCC4_VERB_CFLAGS := -fstrict-overflow -pedantic -std=c99 -Waddress -Waggressive-loop-optimizations -Wall -Wbad-function-cast -Wcast-align -Wcast-qual -Wc++-compat -Wchar-subscripts -Wclobbered -Wcomment -Wconversion -Wcoverage-mismatch -Wdeclaration-after-statement -Wdisabled-optimization -Wdouble-promotion -Wempty-body -Wenum-compare -Wextra -Wfloat-equal -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wignored-qualifiers -Wimplicit -Wimplicit-function-declaration -Wimplicit-int -Winit-self -Winline -Winvalid-pch -Wjump-misses-init -Wlogical-op -Wmain -Wmaybe-uninitialized -Wmissing-braces -Wmissing-declarations -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-parameter-type -Wmissing-prototypes -Wnarrowing -Wnonnull -Wold-style-declaration -Wold-style-definition -Woverlength-strings -Wpacked -Wpacked-bitfield-compat -Wparentheses -Wpedantic -Wpointer-arith -Wpointer-sign -Wredundant-decls -Wreturn-type -Wsequence-point -Wshadow -Wsign-compare -Wsign-conversion -Wsizeof-pointer-memaccess -Wstack-protector -Wstrict-aliasing=1 -Wstrict-overflow=5 -Wstrict-prototypes -Wsuggest-attribute=const -Wsuggest-attribute=format -Wsuggest-attribute=noreturn -Wsuggest-attribute=pure -Wswitch -Wswitch-enum -Wsync-nand -Wtrampolines -Wtrigraphs -Wtype-limits -Wundef -Wuninitialized -Wunknown-pragmas -Wunsafe-loop-optimizations -Wunsuffixed-float-constants -Wunused -Wunused-but-set-parameter -Wunused-but-set-variable -Wunused-function -Wunused-label -Wunused-local-typedefs -Wunused-parameter -Wunused-value -Wunused-variable -Wvariadic-macros -Wvector-operation-performance -Wvla -Wvolatile-register-var -Wwrite-strings -fvisibility=hidden
GCC5_VERB_CXXFLAGS := $(GCC4_VERB_CXXFLAGS) -pedantic-errors -Wabi-tag -Warray-bounds=2 -Wbool-compare -Wc++11-compat -Wc++14-compat -Wconditionally-supported -Wconversion-null -Wctor-dtor-privacy -Wdate-time -Wdelete-incomplete -Wdelete-non-virtual-dtor -Wfatal-errors -Wfloat-conversion  -Wformat-signedness -Wliteral-suffix -Wlogical-not-parentheses -Wmemset-transposed-args -Wnoexcept -Wnon-virtual-dtor -Wnormalized -Wodr -Wopenmp-simd -Woverloaded-virtual -Wreorder -Wshift-count-negative -Wshift-count-overflow -Wsign-promo -Wsizeof-array-argument -Wstrict-null-sentinel -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-bool -Wvariadic-macros -Wno-aggregate-return -Wno-useless-cast -Wno-system-headers -Wno-zero-as-null-pointer-constant -Wno-padded -Wno-switch-default -Wno-float-conversion -Wno-date-time -Wno-effc++
GCC5_VERB_CFLAGS := $(filter-out -pedantic,$(GCC4_VERB_CFLAGS)) -Warray-bounds=2 -Wbool-compare -Wdate-time -Wfatal-errors -Wfloat-conversion -Wformat-signedness -Wincompatible-pointer-types -Wlogical-not-parentheses -Wmemset-transposed-args -Wnested-externs -Wnormalized -Wodr -Wopenmp-simd -Wshift-count-negative -Wshift-count-overflow -Wsizeof-array-argument -Wsuggest-final-methods -Wsuggest-final-types -Wswitch-bool -Wno-aggregate-return -Wno-padded -Wno-switch-default -Wno-float-conversion -Wno-date-time
GCC6_VERB_CXXFLAGS := $(filter-out -Winline,$(GCC5_VERB_CXXFLAGS)) -Wduplicated-cond -Wmisleading-indentation -Wnull-dereference -Wshift-negative-value -Wshift-overflow=2 -Wtautological-compare
GCC6_VERB_CFLAGS := $(filter-out -Winline,$(GCC5_VERB_CFLAGS)) -Wduplicated-cond -Wmisleading-indentation -Wnull-dereference -Wshift-negative-value -Wshift-overflow=2 -Wtautological-compare -Woverride-init-side-effects
CLANG_VERB_CFLAGS := -Weverything -Wno-padded -Wno-cast-align -Wno-reserved-id-macro -Wno-extended-offsetof -Wno-covered-switch-default -Wno-documentation -Wno-assume -Wno-disabled-macro-expansion
CLANG_VERB_CXXFLAGS := -Weverything -Wno-padded -Wno-old-style-cast -Wno-cast-align -Wno-reserved-id-macro -Wno-extended-offsetof -Wno-covered-switch-default -Wno-documentation -Wno-c++11-long-long -Wno-variadic-macros -Wno-assume -Wno-unused-member-function -Wno-disabled-macro-expansion
VERB_CXXFLAGS := $($(COMPILER_TYPE)_VERB_CXXFLAGS)
VERB_CFLAGS   := $($(COMPILER_TYPE)_VERB_CFLAGS)
ifdef CXXCC
VERB_CFLAGS := -x c++ $(VERB_CXXFLAGS)
VERB_SHARED_LIBS := -lstdc++
endif
ifndef DEF_CFLAGS
DEF_CFLAGS := $(VERB_CFLAGS)
DEF_SHARED_LIBS += $(VERB_SHARED_LIBS)
endif
ifndef DEF_CXXFLAGS
DEF_CXXFLAGS := $(VERB_CXXFLAGS)
endif
endif # LINUX

ifeq (SOLARIS,$(OS))
#DEF_CFLAGS := -erroff=badargtype2w,wbadasg,wbadinit
endif

# major.minor.patch
PRODUCT_VER := 1.0.2

# header generated by $(TOP)/version/version.mk
# $(PRODUCT_NAMES_H) - used in $(MTOP)/WINXX/cres.mk
PRODUCT_NAMES_H := vers.h

# version info for a dll
VENDOR_NAME           := Michael M. Builov
VENDOR_URL            := https://github.com/mbuilov/memstack
PRODUCT_NAME          := Memory Stack allocation library
VENDOR_COPYRIGHT      := Copyright (C) 2008-2017 $(VENDOR_NAME), $(VENDOR_URL)

# licence for generated pkgconfig .pc file
PRODUCT_LICENCE       := LGPL version 2.1 or any later version

# library name
MEMSTACK_LIB_NAME ?= memstack$(if $(DEBUG),d)

# variants of built static library
#  LINUX:
#   R,S   - position-dependent code
#   P     - position-independent code for executables
#   D     - position-independent code for shared objects (dlls)
#  WINDOWS:
#   R,P,D - dynamically linked multi-threaded libc
#   S     - statically linked multi-threaded libc
MEMSTACK_LIB_VARIANTS ?= R P D S

# variants of built dynamic library
#  LINUX:
#   R,S   - position-independent code
#  WINDOWS:
#   R     - dynamically linked multi-threaded libc
#   S     - statically linked multi-threaded libc
MEMSTACK_DLL_VARIANTS ?= R S
