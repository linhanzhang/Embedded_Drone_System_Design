COMPONENT_NAME=state

SRC_FILES = \
  $(PROJECT_SRC_DIR)/state/state.c \
  $(UNITTEST_ROOT)/fakes/fake_sensors.c \
  $(UNITTEST_ROOT)/mocks/mock_fail.c \
  $(UNITTEST_ROOT)/mocks/mock_modes.c \
  $(UNITTEST_ROOT)/mocks/mock_barometer.c \
  $(UNITTEST_ROOT)/mocks/mock_mpu6050.c \
  $(UNITTEST_ROOT)/mocks/mock_adc.c \


TEST_SRC_FILES = \
  $(UNITTEST_SRC_DIR)/check_state.c

include $(CHECK_MAKEFILE_INFRA)
