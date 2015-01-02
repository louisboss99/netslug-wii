WD := $(dir $(lastword $(MAKEFILE_LIST)))

SRC += $(WD)settings.c