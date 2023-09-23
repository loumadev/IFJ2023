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
TEST_LIB_DIR = lib/Unity/src

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

# Build target
build: create_build_dirs $(OBJS) $(PROD_MAIN_OBJ)
	$(COMPILER) $(CFLAGS) -I$(INCLUDE_DIR) -o $(BIN_DIR)/$(OUT) $(OBJS) $(PROD_MAIN_OBJ) $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HDRS)
	$(COMPILER) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(PROD_MAIN_OBJ): $(SRC_DIR)/$(PROD_MAIN).c $(HDRS)


# Test target
create_test_main:
	node test/register_tests.js

build_test: create_build_dirs create_test_main $(OBJS) $(TEST_OBJS) $(TEST_MAIN_OBJ)
	$(COMPILER) $(CFLAGS) -I$(INCLUDE_DIR) -I$(TEST_DIR) -o $(BIN_DIR)/test $(OBJS) $(TEST_OBJS) $(TEST_MAIN_OBJ) $(LIBS)

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c $(HDRS) $(TEST_HDRS)
	$(COMPILER) $(CFLAGS) -I$(INCLUDE_DIR) -I$(TEST_DIR) -c $< -o $@

$(TEST_MAIN_OBJ): $(TEST_DIR)/$(TEST_MAIN).c $(HDRS)


# Create build directories for subdirectories in src/
create_build_dirs:
	@mkdir -p $(dir $(OBJS))


run: build
	$(BIN_DIR)/$(OUT)

test: build_test
	$(BIN_DIR)/test


# Clean target
clean:
	rm -rf $(BUILD_DIR)/*.o $(BIN_DIR)/*


.PHONY: build test clean
