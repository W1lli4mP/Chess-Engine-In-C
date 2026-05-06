# compiler and flags
CC = gcc
CFLAGS = -g -Wall -Wextra -Iinclude

# directories
BUILD_DIR = build
BIN_DIR = bin

# executable names
TARGET = $(BIN_DIR)/chess
TEST_FEN_TARGET = $(BIN_DIR)/test_fen

# source files
SRCS = $(wildcard src/*.c)
MAIN_SRC = src/main.c
COMMON_SRCS = $(filter-out $(MAIN_SRC), $(SRCS))
TEST_FEN_SRC = tests/test_fen.c

# object files
OBJS = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRCS))
COMMON_OBJS = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(COMMON_SRCS))
TEST_FEN_OBJ = $(BUILD_DIR)/test_fen.o

# default target
all: $(TARGET)

# main program
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# fen test executable
$(TEST_FEN_TARGET): $(COMMON_OBJS) $(TEST_FEN_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TEST_FEN_TARGET) $(COMMON_OBJS) $(TEST_FEN_OBJ)

# compile source objects
$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# compile test objects
$(BUILD_DIR)/test_fen.o: $(TEST_FEN_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# clean object files and executables
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# run main program
run: $(TARGET)
	./$(TARGET)

# run fen tests
run_fen: $(TEST_FEN_TARGET)
	./$(TEST_FEN_TARGET)

# valgrind main program
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

# valgrind fen tests
valgrind_fen: $(TEST_FEN_TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TEST_FEN_TARGET)