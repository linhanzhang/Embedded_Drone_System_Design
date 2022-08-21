COMPONENT_NAME=filter

SRC_FILES = \
  $(PROJECT_SRC_DIR)/control.c \
  $(UNITTEST_ROOT)/fakes/fake_state.c


TEST_SRC_FILES = \
  $(UNITTEST_SRC_DIR)/check_filter.c

LIBS = -lfftw3
include $(CHECK_MAKEFILE_INFRA)