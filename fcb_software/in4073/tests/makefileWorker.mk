## ----------------------------------------------------------------------
## makeFileWorker.mk
##
##
## Creates a test executable
##
## Author: Koen Wosten
##
## -------
## Inputs - variables desribing what to build
## 	
##	SRC_FILES - source file to build in library
##	TEST_SRC_FILES = source file to build into the unit test runner.
##	MOCKS_SRC_DIRS - Derectories containmock source files to build into the test runner
##
## -------
##  Variables
##
##  	TEST_TARGET - name of the test executable.
##
## 
## ----------------------------------------------------------------------

AR = ar rcs
CC = cc
CFLAGS = -Wall -Wextra -Wpedantic

TARGET_LIB = \
	$(CHECK_LIB_DIR)/lib$(COMPONENT_NAME).a

SRC += $(SRC_FILES)
SRC_OBJ = $(SRC:.c=.o)

TESTS   += $(TEST_SRC_FILES)
TEST_OBJ = $(TESTS:.c=.o)

MOCKS += $(MOCKS_SRC_DIRS)
MOCKS_OBJ = $(MOCKS:.c=.o)

TRASH    = $(wildcard *.gcda) $(wildcard *.gcno) $(wildcard *.gcov)



.PHONY: all
all: start check $(TEST_TARGET)	

.PHONY: check
check: $(TEST_TARGET)
	$(TEST_TARGET)

.PHONY: check-mem
check-mem: $(TEST_TARGET)
	@echo
	@echo VALGRIND:
	valgrind --leak-check=full $(TEST_TARGET)

.PHONY: start
start: $(TEST_TARGET)

TEST_DEPS = $(TEST_OBJ) $(MOCKS_OBJ) $(TARGET_LIB)
test-deps: $(TEST_DEPS)

$(TEST_TARGET) : $(TEST_DEPS)
	@echo 
	@echo LINKING $@
	@mkdir -p $(@D)
	$(CC) -o $@ $^ -lcheck -lm -lpthread -lrt $(LIBS) $(INC_PATHS)

$(TARGET_LIB):$(SRC_OBJ)
	@echo 
	@echo Building archive $@
	@mkdir -p $(dir $@)
	$(AR) $@ $^

%.o: %.c
	@echo 
	@echo Compiling $(notdir $<)
	mkdir -p $(@D)
	$(CC) -o $@ -c $^ $(CFLAGS) $(INC_PATHS)

clean: 
	rm -rf $(SRC_OBJ) $(TEST_OBJ) $(MOCKS_OBJ) $(TRASH)
	
