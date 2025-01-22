# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -Iinclude

# Directories
BUILD_DIR = build
SRC_DIR = src
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
INCLUDE_DIR = include
OUTPUT_DIR = barcode-output

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)

# Object files (corresponding .o files in obj/)
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Executables
EXEC_GEN = $(BIN_DIR)/gen
EXEC_EXT = $(BIN_DIR)/ext

# Default rule (all targets)
all: $(EXEC_GEN) $(EXEC_EXT)

# Rule to create the 'gen' executable
$(EXEC_GEN): $(OBJ_DIR)/EAN-8_code_generator.o $(OBJ_DIR)/funcs.o $(OBJ_DIR)/io.o $(OBJ_DIR)/definitions.o
	$(CC) -o $@ $^

# Rule to create the 'ext' executable
$(EXEC_EXT): $(OBJ_DIR)/EAN-8_code_extractor.o $(OBJ_DIR)/funcs.o $(OBJ_DIR)/io.o $(OBJ_DIR)/definitions.o
	$(CC) -o $@ $^

# Rule to compile .c files into .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)  # Ensure obj directory exists
	$(CC) $(CFLAGS) -c $< -o $@

# Create the necessary directories (obj, bin, output)
	mkdir -p $(OBJ_DIR) $(BIN_DIR) $(OUTPUT_DIR);


# Rule to create symlinks in the root directory for executables
	ln -sf $(EXEC_GEN) ./gen
	ln -sf $(EXEC_EXT) ./ext

# Clean rule to remove the build directory
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean