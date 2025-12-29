CC      := gcc
CFLAGS  := -Wall -Wextra -Wpedantic -std=c11 -g -Og -MMD -MP -D_POSIX_C_SOURCE=200809L

SRC_DIR := src
BLD_DIR := build

TARGET  := ccd

SRCS := $(SRC_DIR)/main.c \
        $(SRC_DIR)/utils.c \
		$(SRC_DIR)/parse.c

OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BLD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all clean

all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile
$(BLD_DIR)/%.o: $(SRC_DIR)/%.c | $(BLD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure build dir exists
$(BLD_DIR):
	mkdir -p $(BLD_DIR)

clean:
	rm -rf $(BLD_DIR)

# Auto-generated header deps
-include $(DEPS)

