# Directories
SRC_DIR := labwork3
BUILD_DIR := bin

# Files
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Compiler
CC := gcc
CFLAGS := -Wall -I$(SRC_DIR)

# Target
TARGET := $(BUILD_DIR)/labwork3

# Default target
labwork3: $(TARGET)

# Link the final binary
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR)/*.o $(TARGET)
