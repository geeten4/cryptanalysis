# static
CC := gcc

# ------ LABWORK 1

# directories
SRC_DIR1 := labwork1/src
CFLAGS1 := -Wall -I$(SRC_DIR1)
BUILD_DIR1 := bin/labwork1

# files
SRCS1 := $(wildcard $(SRC_DIR1)/*.c)
OBJS1 := $(patsubst $(SRC_DIR1)/%.c, $(BUILD_DIR1)/%.o, $(SRCS1))

# target
TARGET1 := $(BUILD_DIR1)/labwork1

# default target
.PHONY: labwork1
labwork1: $(TARGET1)

# link the final binary
$(TARGET1): $(OBJS1)
	$(CC) $(OBJS1) -o $@

# compile source files to object files
$(BUILD_DIR1)/%.o: $(SRC_DIR1)/%.c
	@mkdir -p $(BUILD_DIR1)
	$(CC) $(CFLAGS1) -c $< -o $@

# ------ LABWORK 2

# directories
SRC_DIR2 := labwork2/src
CFLAGS2 := -Wall -I$(SRC_DIR2)
BUILD_DIR2 := bin/labwork2

# files
SRCS2 := $(wildcard $(SRC_DIR2)/*.c)
OBJS2 := $(patsubst $(SRC_DIR2)/%.c, $(BUILD_DIR2)/%.o, $(SRCS2))

# target
TARGET2 := $(BUILD_DIR2)/labwork2

# default target
.PHONY: labwork2
labwork2: $(TARGET2)

# link the final binary
$(TARGET2): $(OBJS2)
	$(CC) $(OBJS2) -o $@

# compile source files to object files
$(BUILD_DIR2)/%.o: $(SRC_DIR2)/%.c
	@mkdir -p $(BUILD_DIR2)
	$(CC) $(CFLAGS2) -c $< -o $@

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



# ------ LABWORK FINAL


# directories
SRC_DIR_final := labwork_final/src
CFLAGS_final := -Wall -I$(SRC_DIR_final)
BUILD_DIR_final := bin/labwork_final

# files
SRCS_final := $(wildcard $(SRC_DIR_final)/*.c)
OBJS_final := $(patsubst $(SRC_DIR_final)/%.c, $(BUILD_DIR_final)/%.o, $(SRCS_final))

# target
TARGET_final := $(BUILD_DIR_final)/labwork_final

# default target
.PHONY: labwork_final
labwork_final: $(TARGET_final)

# link the final binary
$(TARGET_final): $(OBJS_final)
	$(CC) $(OBJS_final) -o $@ -lm

# compile source files to object files
$(BUILD_DIR_final)/%.o: $(SRC_DIR_final)/%.c
	@mkdir -p $(BUILD_DIR_final)
	$(CC) $(CFLAGS_final) -c $< -o $@

# shared dl.c for python scripts
DL_TARGET := $(BUILD_DIR_final)/dl.so
DL_SRCS := $(SRC_DIR_final)/dl.c $(SRC_DIR_final)/factor_basis.c $(SRC_DIR_final)/blist.c $(SRC_DIR_final)/gf.c $(SRC_DIR_final)/vector.c $(SRC_DIR_final)/matrix.c $(SRC_DIR_final)/utils.c
DL_OBJS := $(patsubst $(SRC_DIR_final)%.c, $(BUILD_DIR_final)%_shared.o, $(DL_SRCS))

# generate shared library of dl functions for python

.PHONY: dl
dl: dl.so

dl.so: $(DL_TARGET)

$(DL_TARGET): $(DL_OBJS)
	$(CC) $(LDFLAGS_SHARED) -o $@ $^

$(BUILD_DIR_final)/%_shared.o: $(SRC_DIR_final)/%.c
	$(CC) $(CFLAGS_SHARED) -c $< -o $@



# cleaning
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR3)/*.o $(BUILD_DIR3)/*.so $(TARGET3) $(BUILD_DIR4)/*.o $(TARGET4) $(BUILD_DIR5)/*.o $(TARGET5) $(BUILD_DIR_FINAL)/*.o $(TARGET_FINAL) $(DL_TARGET)
