# compiler and flags
CC = gcc
CFLAGS = -g -Wall -Wextra

# executable name
TARGET = chess

# source files
SRCS = main.c board.c piece.c parser.c move.c queue.c linked_list.c

# object files
OBJS = $(SRCS:.c=.o)

# default target
all: $(TARGET)

# compile program
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# compile object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# clean object files and executable
clean:
	rm -f $(OBJS) $(TARGET)

# run program
run: $(TARGET)
	./$(TARGET)

# run program with valgrind
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)