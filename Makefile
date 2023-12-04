##
# File: Makefile
# Author: Jaroslav Louma (xlouma00@stud.fit.vutbr.cz)
# Author: Radim Mifka (xmifka00@stud.fit.vutbr.cz)
# Date: 2023-09-01
# Brief: This file is part of the IFJ23 project.
##

#
# Options
#

# Compiler and flags
COMPILER = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror -g
LIBS = -lm
OUT = main

# Entry points
PROD_MAIN = main
TEST_MAIN = test_main

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin
TEST_DIR = test

#
# Internal variables
#

# Source, header object files
PROD_MAIN_OBJ = $(BUILD_DIR)/$(PROD_MAIN).o
HDRS = $(shell find $(INCLUDE_DIR) -name "*.h")
SRCS = $(shell find $(SRC_DIR) -name "*.c" ! -name "$(PROD_MAIN).c")
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Test source, header object files
TEST_MAIN_OBJ = $(BUILD_DIR)/$(TEST_MAIN).o
TEST_HDRS = $(shell find $(INCLUDE_DIR) -name "*.h")
TEST_SRCS = $(shell find $(TEST_DIR) -name "*.c" ! -name "$(TEST_MAIN).c")
TEST_OBJS = $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%.o, $(TEST_SRCS))

#
# Targets
#

## Default target

all: build test deploy clean


## Production build

# Builds the project with production entry point
build: create_output_dirs $(OBJS) $(PROD_MAIN_OBJ)
	$(COMPILER) $(CFLAGS) -I$(INCLUDE_DIR) -o $(BIN_DIR)/$(OUT) $(OBJS) $(PROD_MAIN_OBJ) $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HDRS)
	$(COMPILER) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(PROD_MAIN_OBJ): $(SRC_DIR)/$(PROD_MAIN).c $(HDRS)


## Test build

# Builds the project with test entry point
build_test: create_output_dirs create_test_main $(OBJS) $(TEST_OBJS) $(TEST_MAIN_OBJ)
	$(COMPILER) $(CFLAGS) -I$(INCLUDE_DIR) -I$(TEST_DIR) -o $(BIN_DIR)/test $(OBJS) $(TEST_OBJS) $(TEST_MAIN_OBJ) $(LIBS)

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c $(HDRS) $(TEST_HDRS)
	$(COMPILER) $(CFLAGS) -I$(INCLUDE_DIR) -I$(TEST_DIR) -c $< -o $@

$(TEST_MAIN_OBJ): $(TEST_DIR)/$(TEST_MAIN).c $(HDRS)


## Helper targets

# Generates a test entry point
create_test_main:
	node test/register_tests.js

# Creates the directories required for building
create_output_dirs:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(dir $(OBJS))


## Public targets

# Builds and runs the project
run: build
	$(BIN_DIR)/$(OUT)

# Builds and runs the tests
test: build_test
	$(BIN_DIR)/test

# Deploys the project
deploy:
	node deploy/deploy.js

# Cleans the build files
clean:
	rm -rf $(BUILD_DIR)/*.o $(BIN_DIR)/*


## Phony targets

.PHONY: all build build_test create_test_main create_output_dirs run test deploy clean

# End of file Makefile
