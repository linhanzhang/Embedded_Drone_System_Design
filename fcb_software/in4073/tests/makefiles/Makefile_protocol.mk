COMPONENT_NAME=protocol

SRC_FILES = \
  $(PROJECT_SRC_DIR)/comms/packet.c 

TEST_SRC_FILES = \
  $(UNITTEST_SRC_DIR)/check_protocol.c

include $(CHECK_MAKEFILE_INFRA)
