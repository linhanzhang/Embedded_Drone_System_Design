COMPONENT_NAME=packet_builder

SRC_FILES = \
  $(PROJECT_SRC_DIR)/comms/packet.c \
    $(PROJECT_SRC_DIR)/comms/packet_builder.c 


TEST_SRC_FILES = \
  $(UNITTEST_SRC_DIR)/check_packet_builder.c

include $(CHECK_MAKEFILE_INFRA)
