include $(dir $(lastword $(MAKEFILE_LIST)))../top.mk
TOOL_MODE := 1
include $(MTOP)/c.mk
EXE        := buildnumber S
SRC        := buildnumber.c
CMNDEFINES :=
CMNINCLUDE :=
$(DEFINE_TARGETS)
