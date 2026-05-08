# compiler and flags
CC = gcc
CFLAGS = -g -Wall -Wextra -Iinclude

# directories
BUILD_DIR = build
BIN_DIR = bin

# executable names
TARGET = $(BIN_DIR)/chess
TEST_FEN_TARGET = $(BIN_DIR)/test_fen
TEST_MOVE_GEN_TARGET = $(BIN_DIR)/test_move_gen
TEST_MOVE_APPLY_TARGET = $(BIN_DIR)/test_move_apply
TEST_SAN_RESOLVE_TARGET = $(BIN_DIR)/test_san_resolve

# source files
SRCS = $(wildcard src/*.c)
MAIN_SRC = src/main.c
COMMON_SRCS = $(filter-out $(MAIN_SRC), $(SRCS))

TEST_FEN_SRC = tests/test_fen.c
TEST_MOVE_GEN_SRC = tests/test_move_gen.c
TEST_MOVE_APPLY_SRC = tests/test_move_apply.c
TEST_SAN_RESOLVE_SRC = tests/test_san_resolve.c

# object files
OBJS = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRCS))
COMMON_OBJS = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(COMMON_SRCS))

TEST_UTILS_OBJ = $(BUILD_DIR)/test_utils.o
TEST_FEN_OBJ = $(BUILD_DIR)/test_fen.o
TEST_MOVE_GEN_OBJ = $(BUILD_DIR)/test_move_gen.o
TEST_MOVE_APPLY_OBJ = $(BUILD_DIR)/test_move_apply.o
TEST_SAN_RESOLVE_OBJ = $(BUILD_DIR)/test_san_resolve.o

# default target
all: $(TARGET)

# main program
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# FEN test executable
$(TEST_FEN_TARGET): $(COMMON_OBJS) $(TEST_FEN_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TEST_FEN_TARGET) $(COMMON_OBJS) $(TEST_FEN_OBJ)

# move generation test executable
$(TEST_MOVE_GEN_TARGET): $(COMMON_OBJS) $(TEST_UTILS_OBJ) $(TEST_MOVE_GEN_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TEST_MOVE_GEN_TARGET) $(COMMON_OBJS) $(TEST_UTILS_OBJ) $(TEST_MOVE_GEN_OBJ)

# move apply test executable
$(TEST_MOVE_APPLY_TARGET): $(COMMON_OBJS) $(TEST_MOVE_APPLY_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TEST_MOVE_APPLY_TARGET) $(COMMON_OBJS) $(TEST_MOVE_APPLY_OBJ)

# SAN resolve test executable
$(TEST_SAN_RESOLVE_TARGET): $(COMMON_OBJS) $(TEST_UTILS_OBJ) $(TEST_SAN_RESOLVE_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TEST_SAN_RESOLVE_TARGET) $(COMMON_OBJS) $(TEST_UTILS_OBJ) $(TEST_SAN_RESOLVE_OBJ)

# compile source objects
$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# compile test objects
$(BUILD_DIR)/test_utils.o: tests/test_utils.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_fen.o: $(TEST_FEN_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_move_gen.o: $(TEST_MOVE_GEN_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_move_apply.o: $(TEST_MOVE_APPLY_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_san_resolve.o: $(TEST_SAN_RESOLVE_SRC) | $(BUILD_DIR)
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

# run FEN tests
run_fen: $(TEST_FEN_TARGET)
	./$(TEST_FEN_TARGET)

# run move generation tests
run_move_gen: $(TEST_MOVE_GEN_TARGET)
	./$(TEST_MOVE_GEN_TARGET)

# run move apply tests
run_move_apply: $(TEST_MOVE_APPLY_TARGET)
	./$(TEST_MOVE_APPLY_TARGET)

# run SAN resolve tests
run_san_resolve: $(TEST_SAN_RESOLVE_TARGET)
	./$(TEST_SAN_RESOLVE_TARGET)

# run all tests
test: run_fen run_move_gen run_move_apply run_san_resolve

# valgrind main program
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

# valgrind FEN tests
valgrind_fen: $(TEST_FEN_TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TEST_FEN_TARGET)

# valgrind move generation tests
valgrind_move_gen: $(TEST_MOVE_GEN_TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TEST_MOVE_GEN_TARGET)

# valgrind move apply tests
valgrind_move_apply: $(TEST_MOVE_APPLY_TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TEST_MOVE_APPLY_TARGET)

# valgrind SAN resolve tests
valgrind_san_resolve: $(TEST_SAN_RESOLVE_TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TEST_SAN_RESOLVE_TARGET)

.PHONY: all clean run run_fen run_move_gen run_move_apply run_san_resolve test valgrind valgrind_fen valgrind_move_gen valgrind_move_apply valgrind_san_resolve