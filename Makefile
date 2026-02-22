# compiler and flags
CC = gcc
CFLAGS = -g -Wall -Wextra -Iinclude

# executable name
TARGET = chess
BUILD_DIR = build

# source files
SRCS = $(wildcard src/*.c)

# object files
OBJS = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# default target
all: $(TARGET)

# compile program
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# compile object files
$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# clean object files and executable
clean:
	rm -f $(OBJS) $(TARGET)

# run program
run: $(TARGET)
	./$(TARGET)

# run program with valgrind
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)