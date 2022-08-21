## ----------------------------------------------------------------------
## 
## Makefile with standard overrides for the automatic test suite
##
## Author: Koen Wosten
## ----------------------------------------------------------------------


TEST_SRC_FILES += \
  $(UNITTEST_SRC_DIR)/check_check.c

UNITTEST_RESULT_DIR=$(UNITTEST_BUILD)/$(COMPONENT_NAME)
export TEST_TARGET=$(UNITTEST_RESULT_DIR)/$(COMPONENT_NAME)_tests

CHECK_LIB_DIR = $(UNITTEST_RESULT_DIR)/lib

#includes common to all targets
INC_PATHS += -I$(abspath ./header_overwrites)
INC_PATHS += -I$(abspath .././)
INC_PATHS += -I$(abspath .././mpu6050)
INC_PATHS += -I$(abspath .././utils/config)
INC_PATHS += -I$(abspath .././utils)
INC_PATHS += -I$(abspath .././state/modes)
INC_PATHS += -I$(abspath .././state)
INC_PATHS += -I$(abspath .././comms)
INC_PATHS += -I$(abspath .././hal)
INC_PATHS += -I$(abspath ../../components/device)
INC_PATHS += -I$(abspath ../../components/toolchain/gcc)
INC_PATHS += -I$(abspath ../../components/toolchain)
INC_PATHS += -I$(abspath ../../components/drivers_nrf/hal)
INC_PATHS += -I$(abspath ../../components/drivers_nrf/delay)
INC_PATHS += -I$(abspath ../../components/softdevice/s110/headers)
INC_PATHS += -I$(abspath ../../components/drivers_nrf/config)
INC_PATHS += -I$(abspath ../../components/libraries/util)
INC_PATHS += -I$(abspath ../../components/ble/common)
INC_PATHS += -I$(abspath ../../components/drivers_nrf/pstorage)
INC_PATHS += -I$(abspath ../../components/libraries/timer)
INC_PATHS += -I$(abspath ../../components/ble/ble_services/ble_nus)
INC_PATHS += -I$(abspath ../../components/drivers_nrf/common)
INC_PATHS += -I$(abspath ../../components/ble/ble_advertising)
INC_PATHS += -I$(abspath ../../components/libraries/trace)
INC_PATHS += -I$(abspath ../../components/softdevice/common/softdevice_handler)
# run MakefileWorker.mk with the variables defined here
include makefileWorker.mk
