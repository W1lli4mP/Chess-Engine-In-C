CC = gcc
CFLAGS = -Wall -Wextra

TARGET = main
SOURCES = main.c board.c piece.c move.c rules.c

all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)