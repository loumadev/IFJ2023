# Compiler and flags
COMPILER = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror -g
LIBS = -lm
OUT = main

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin
TEST_DIR = test

# Source files and header files
# SRCS = $(wildcard $(SRC_DIR)/**/*.c $
SRCS = $(shell find $(SRC_DIR) -name "*.c")
HDRS = $(shell find $(INCLUDE_DIR) -name "*.h")

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Test files
TEST_SRCS = $(shell find $(TEST_DIR) -name "*.c")
TEST_OBJS = $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%.o, $(TEST_SRCS))


# Build target
build: create_build_dirs $(OBJS)
	$(COMPILER) $(CFLAGS) -I$(INCLUDE_DIR) -o $(BIN_DIR)/$(OUT) $(OBJS) $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HDRS)
	$(COMPILER) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@


# Test target
test: create_build_dirs $(TEST_OBJS) $(OBJS)
	$(COMPILER) $(CFLAGS) -I$(INCLUDE_DIR) -o $(BIN_DIR)/test $(TEST_OBJS) $(OBJS) $(LIBS)
	$(BIN_DIR)/test

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c $(HDRS)
	$(COMPILER) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@


# Create build directories for subdirectories in src/
create_build_dirs:
	@mkdir -p $(dir $(OBJS))


run: build
	$(BIN_DIR)/$(OUT)


# Clean target
clean:
	rm -rf $(BUILD_DIR)/*.o $(BIN_DIR)/*


.PHONY: build test clean
