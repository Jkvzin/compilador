CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = -lm

SRC_DIR = src
OBJ_DIR = .obj
BIN_DIR = .

# Source files (excluding runtime_io which is no longer needed)
SOURCES = $(SRC_DIR)/token.c $(SRC_DIR)/lexer.c $(SRC_DIR)/ast.c \
          $(SRC_DIR)/symbol.c $(SRC_DIR)/parser.c $(SRC_DIR)/semantic.c \
          $(SRC_DIR)/codegen.c $(SRC_DIR)/main.c
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

# Executable
TARGET = $(BIN_DIR)/simplesc

# Default target
all: $(OBJ_DIR) $(TARGET)

# Create object directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link main executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Compile examples
examples: $(TARGET)
	@mkdir -p output
	./$(TARGET) compile examples/ex1.simp output/ex1.asm
	./$(TARGET) compile examples/ex2.simp output/ex2.asm
	./$(TARGET) compile examples/ex3.simp output/ex3.asm
	./$(TARGET) compile examples/hello.simples output/hello.asm
	./$(TARGET) compile examples/fatorial.simples output/fatorial.asm
	./$(TARGET) compile examples/fibonacci.simples output/fibonacci.asm
	@echo "Todos os exemplos compilados com sucesso!"

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(TARGET) output/

.PHONY: all examples clean
