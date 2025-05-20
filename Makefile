# directories
SRC_DIR := labwork3/src
BUILD_DIR := bin

# files
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# static
CC := gcc
CFLAGS := -Wall -I$(SRC_DIR)

# shared
CFLAGS_SHARED = -Wall -fPIC -I$(SRC_DIR)
LDFLAGS_SHARED = -shared

# target
TARGET := $(BUILD_DIR)/labwork3

# default target
labwork3: $(TARGET)

# link the final binary
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

# compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# shared encryption.c for python scripts
ENCRYPTION_TARGET := $(BUILD_DIR)/encryption.so
ENCRYPTION_SRCS := $(SRC_DIR)/encryption.c $(SRC_DIR)/gf.c $(SRC_DIR)/permutation.c
ENCRYPTION_OBJS := $(patsubst $(SRC_DIR)%.c, $(BUILD_DIR)%_shared.o, $(ENCRYPTION_SRCS))

encryption.so: $(ENCRYPTION_TARGET)

$(ENCRYPTION_TARGET): $(ENCRYPTION_OBJS)
	$(CC) $(LDFLAGS_SHARED) -o $@ $^

$(BUILD_DIR)/%_shared.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS_SHARED) -c $< -o $@

# cleaning
clean:
	rm -rf $(BUILD_DIR)/*.o $(TARGET)
	rm -rf $(BUILD_DIR)/*.so $(TARGET)
