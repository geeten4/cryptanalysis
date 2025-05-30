# static
CC := gcc

# ------ LABWORK 3

# directories
BUILD_DIR3 := bin/labwork3
SRC_DIR3 := labwork3/src
CFLAGS3 := -Wall -I$(SRC_DIR3)

# files
SRCS3 := $(wildcard $(SRC_DIR3)/*.c)
OBJS3 := $(patsubst $(SRC_DIR3)/%.c, $(BUILD_DIR3)/%.o, $(SRCS3))

# shared
CFLAGS_SHARED = -Wall -fPIC -I$(SRC_DIR3)
LDFLAGS_SHARED = -shared

# target
TARGET3 := $(BUILD_DIR3)/labwork3

# default target
.PHONY: labwork3
labwork3: $(TARGET3)

# link the final binary
$(TARGET3): $(OBJS3)
	$(CC) $(OBJS3) -o $@

# compile source files to object files
$(BUILD_DIR3)/%.o: $(SRC_DIR3)/%.c
	@mkdir -p $(BUILD_DIR3)
	$(CC) $(CFLAGS3) -c $< -o $@

# shared encryption.c for python scripts
ENCRYPTION_TARGET := $(BUILD_DIR3)/encryption.so
ENCRYPTION_SRCS := $(SRC_DIR3)/encryption.c $(SRC_DIR3)/gf.c $(SRC_DIR3)/permutation.c
ENCRYPTION_OBJS := $(patsubst $(SRC_DIR3)%.c, $(BUILD_DIR3)%_shared.o, $(ENCRYPTION_SRCS))

# generate shared library of encryption functions for python

.PHONY: encryption
encryption: encryption.so

encryption.so: $(ENCRYPTION_TARGET)

$(ENCRYPTION_TARGET): $(ENCRYPTION_OBJS)
	$(CC) $(LDFLAGS_SHARED) -o $@ $^

$(BUILD_DIR3)/%_shared.o: $(SRC_DIR3)/%.c
	$(CC) $(CFLAGS_SHARED) -c $< -o $@


# ------ LABWORK 4


# directories
SRC_DIR4 := labwork4/src
CFLAGS4 := -Wall -I$(SRC_DIR4)
BUILD_DIR4 := bin/labwork4

# files
SRCS4 := $(wildcard $(SRC_DIR4)/*.c)
OBJS4 := $(patsubst $(SRC_DIR4)/%.c, $(BUILD_DIR4)/%.o, $(SRCS4))

# target
TARGET4 := $(BUILD_DIR4)/labwork4

# default target
.PHONY: labwork4
labwork4: $(TARGET4)

# link the final binary
$(TARGET4): $(OBJS4)
	$(CC) $(OBJS4) -o $@

# compile source files to object files
$(BUILD_DIR4)/%.o: $(SRC_DIR4)/%.c
	@mkdir -p $(BUILD_DIR4)
	$(CC) $(CFLAGS4) -c $< -o $@


# ------ LABWORK 5


# directories
SRC_DIR5 := labwork5/src
CFLAGS5 := -Wall -I$(SRC_DIR5)
BUILD_DIR5 := bin/labwork5

# files
SRCS5 := $(wildcard $(SRC_DIR5)/*.c)
OBJS5 := $(patsubst $(SRC_DIR5)/%.c, $(BUILD_DIR5)/%.o, $(SRCS5))

# target
TARGET5 := $(BUILD_DIR5)/labwork5

# default target
.PHONY: labwork5
labwork5: $(TARGET5)

# link the final binary
$(TARGET5): $(OBJS5)
	$(CC) $(OBJS5) -o $@

# compile source files to object files
$(BUILD_DIR5)/%.o: $(SRC_DIR5)/%.c
	@mkdir -p $(BUILD_DIR5)
	$(CC) $(CFLAGS5) -c $< -o $@


# cleaning
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR3)/*.o $(BUILD_DIR3)/*.so $(TARGET3) $(BUILD_DIR4)/*.o $(TARGET4) $(BUILD_DIR5)/*.o $(TARGET5)
