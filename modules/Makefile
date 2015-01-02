###############################################################################
# Makefile
#  by Alex Chadwick
#
# A makefile script for generation of all brainslug modules
###############################################################################

MODULES += netslug_main

###############################################################################
# Parameters

# Used to suppress command echo.
Q      ?= @
LOG    ?= @echo $@

###############################################################################
# Rule to make everything.
PHONY += all
all: $(addsuffix _module,$(MODULES))

%_module:
	$Q$(MAKE) -C $*

###############################################################################
# Release rule

PHONY += release
release: $(addsuffix _module_release,$(MODULES))

%_module_release: %_module
	$(LOG)
	$Qcp $*/bin/$*.mod $(RELEASE_DIR)/$*.mod 

###############################################################################
# Clean rule

# Rule to clean files.
PHONY += clean
clean: $(addsuffix _module_clean,$(MODULES))
	
%_module_clean:
	$Q$(MAKE) -C $* clean

###############################################################################
# Phony targets

.PHONY : $(PHONY)