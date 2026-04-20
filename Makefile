CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -fPIC
LDFLAGS = -lm

SRC_DIR = src
OBJ_DIR = .obj
TEST_DIR = tests
BIN_DIR = .

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

# Test files
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.c, $(OBJ_DIR)/%.o, $(TEST_SOURCES))

# Unity framework
UNITY_DIR = unity
UNITY_SRC = $(UNITY_DIR)/unity.c
UNITY_OBJ = $(OBJ_DIR)/unity.o

# Executable names
TARGET = $(BIN_DIR)/simplesc
TEST_TARGET = $(BIN_DIR)/test_runner

# Default target
all: dirs $(TARGET)

# Create directories
dirs: $(SRC_DIR) $(TEST_DIR) unity $(OBJ_DIR)

$(SRC_DIR) $(TEST_DIR) unity:
	mkdir -p $@

# Create object directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile test files
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(UNITY_DIR) -c $< -o $@

# Compile Unity framework
$(UNITY_OBJ): $(UNITY_SRC) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(UNITY_SRC) -o $(UNITY_OBJ)

# Link main executable (exclude test files)
$(TARGET): $(filter-out $(OBJ_DIR)/test_%.o, $(OBJECTS))
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Link test executable
$(TEST_TARGET): $(TEST_OBJECTS) $(UNITY_OBJ) $(filter-out $(OBJ_DIR)/main.o, $(OBJECTS))
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Run tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(TEST_TARGET)

.PHONY: all test clean
