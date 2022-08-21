## ----------------------------------------------------------------------
## Makefile
##
##
## Launch test cases
##
## Author: Koen Wosten
##
## ----------------------------------------------------------------------


export UNITTEST_BUILD = build
export UNITTEST_ROOT := $(abspath .)
export PROJECT_ROOT := $(abspath ..)
export PROJECT_SRC_DIR := $(PROJECT_ROOT)

export UNITTEST_SRC_DIR := $(UNITTEST_ROOT)/src
export UNITTEST_MAKEFILES_DIR := $(UNITTEST_ROOT)/makefiles
export CHECK_MAKEFILE_INFRA := $(UNITTEST_ROOT)/MakefileWorkerOverrides.mk

export UNITTEST_EXTRA_INC_PATHS += \
	-I$(PROJECT_ROOT)

UNITTEST_MAKEFILES := $(wildcard $(UNITTEST_MAKEFILES_DIR)/Makefile_*)

all: check-mem

compile: BUILD_RULE=start
compile: $(UNITTEST_MAKEFILES)

check: BUILD_RULE=check
check: $(UNITTEST_MAKEFILES)

check-mem:BUILD_RULE=check-mem CK_FORK=NO 
check-mem: $(UNITTEST_MAKEFILES)

$(UNITTEST_MAKEFILES):
	@echo 
	@echo 
	@echo =======================================
	@echo Making $(@F) using $(BUILD_RULE)
	@echo =======================================
	$(MAKE) --no-print-directory -f $@ $(BUILD_RULE)

clean: BUILD_RULE=clean
clean: $(UNITTEST_MAKEFILES)
	rm -rf $(UNITTEST_BUILD)


.PHONY: all clean $(UNITTEST_MAKEFILES)
