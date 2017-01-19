include $(dir $(lastword $(MAKEFILE_LIST)))../top.mk
include $(MTOP)/defs.mk

GENERATED := $(GEN_DIR)/$(PRODUCT_NAMES_H)
$(call ADD_GENERATED,$(GENERATED))

# $$1 - $(word 1,$(BUILDNUMBERS))
# $$2 - $(subst /,-,$(word 2,$(BUILDNUMBERS))) $(word 3,$(BUILDNUMBERS))
define PRODUCT_NAMES_TEMPLATE
#ifndef $(call toupper,$(subst .,_,$(PRODUCT_NAMES_H)))_INCLUDED
#define $(call toupper,$(subst .,_,$(PRODUCT_NAMES_H)))_INCLUDED
#define VENDOR_NAME           "$(VENDOR_NAME)"
#define PRODUCT_NAME          "$(PRODUCT_NAME)"
#define VENDOR_COPYRIGHT      "$(VENDOR_COPYRIGHT)"
#define PRODUCT_VERSION_MAJOR $(PRODUCT_VERSION_MAJOR)
#define PRODUCT_VERSION_MINOR $(PRODUCT_VERSION_MINOR)
#define PRODUCT_VERSION_NUM   $(PRODUCT_VERSION_MAJOR).$(PRODUCT_VERSION_MINOR)
#define PRODUCT_VERSION       "$(PRODUCT_VERSION_MAJOR).$(PRODUCT_VERSION_MINOR)"
#define PRODUCT_OS            "$(OS)"
#define PRODUCT_UCPU          "$(CPU)"
#define PRODUCT_TARGET        "$(TARGET)"
#define PRODUCT_BUILD_NUM     $$1
#define PRODUCT_BUILD         "$$1"
#define PRODUCT_BUILD_DATE    "$$2"
#define PRODUCT_BUILD_VERSION "$(PRODUCT_VERSION_MAJOR).$(PRODUCT_VERSION_MINOR).$$1"
#endif
endef

ifndef PRODUCT_BUILDNUMBERS
$(GENERATED): $(call GET_TOOL,buildnumber)
endif

$(GENERATED): TEMPL := $(PRODUCT_NAMES_TEMPLATE)
$(GENERATED): BUILDNUMBERS := $(PRODUCT_BUILDNUMBERS)
$(GENERATED):
	$(if $(BUILDNUMBERS),,$(eval $@: BUILDNUMBERS := $(shell $(call ospath,$(call GET_TOOL,buildnumber)))))
	$(if $(BUILDNUMBERS),,$(error PRODUCT_BUILDNUMBERS not defined))
	$(call SUP,GEN,$@)$(call ECHO,$(subst $$2,$(subst /,-,$(word 2,$(BUILDNUMBERS))) $(word \
  3,$(BUILDNUMBERS)),$(subst $$1,$(word 1,$(BUILDNUMBERS)),$(TEMPL)))) > $@

$(DEFINE_TARGETS)
