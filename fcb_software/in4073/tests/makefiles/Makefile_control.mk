COMPONENT_NAME=control

SRC_FILES = \
  $(PROJECT_SRC_DIR)/control.c \
  $(UNITTEST_ROOT)/fakes/fake_state.c

TEST_SRC_FILES = \
  $(UNITTEST_SRC_DIR)/check_control.c

include $(CHECK_MAKEFILE_INFRA)
