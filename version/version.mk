include $(dir $(lastword $(MAKEFILE_LIST)))../top.mk
include $(MTOP)/defs.mk

# generate product definitions header
# (this header may be used for $(MTOP)/WINXX/cres.mk)
GENERATED := $(GEN_DIR)/$(PRODUCT_NAMES_H)
$(call ADD_GENERATED,$(GENERATED))

# <build_num>  - $(firstword $(BUILDNUMBERS))
# <build_date> - $(subst /,-,$(word 2,$(BUILDNUMBERS))) $(word 3,$(BUILDNUMBERS))
define PRODUCT_NAMES_TEMPLATE
#ifndef $(call toupper,$(subst .,_,$(PRODUCT_NAMES_H)))_INCLUDED
#define $(call toupper,$(subst .,_,$(PRODUCT_NAMES_H)))_INCLUDED
#define VENDOR_NAME           "$(VENDOR_NAME)"
#define PRODUCT_NAME          "$(PRODUCT_NAME)"
#define VENDOR_COPYRIGHT      "$(VENDOR_COPYRIGHT)"
#define PRODUCT_VERSION_MAJOR $(call ver_major,$(PRODUCT_VER))
#define PRODUCT_VERSION_MINOR $(call ver_minor,$(PRODUCT_VER))
#define PRODUCT_VERSION_PATCH $(call ver_patch,$(PRODUCT_VER))
#define PRODUCT_OS            "$(OS)"
#define PRODUCT_UCPU          "$(UCPU)"
#define PRODUCT_KCPU          "$(KCPU)"
#define PRODUCT_TARGET        "$(TARGET)"
#define PRODUCT_BUILD_NUM     <build_num>
#define PRODUCT_BUILD_DATE    "<build_date>"
#endif
endef

# PRODUCT_BUILDNUMBERS may be defined as: 17160 2017/02/16 10:43:09
ifndef PRODUCT_BUILDNUMBERS
$(GENERATED): $(call GET_TOOL,buildnumber)
endif

$(GENERATED): TEMPL := $(PRODUCT_NAMES_TEMPLATE)
$(GENERATED): BUILDNUMBERS := $(PRODUCT_BUILDNUMBERS)
$(GENERATED):
	$(if $(BUILDNUMBERS),,$(eval $@: BUILDNUMBERS := $(shell $(call ospath,$(call GET_TOOL,buildnumber)))))
	$(if $(BUILDNUMBERS),,$(error PRODUCT_BUILDNUMBERS not defined))
	$(call SUP,GEN,$@)$(call ECHO,$(subst <build_date>,$(subst /,-,$(word 2,$(BUILDNUMBERS))) $(word \
  3,$(BUILDNUMBERS)),$(subst <build_num>,$(firstword $(BUILDNUMBERS)),$(TEMPL)))) > $@

$(DEFINE_TARGETS)
