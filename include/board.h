#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

#include "piece.h"
#include "square.h"

#define BOARD_SIZE 8

typedef struct
{
    Piece *grid[BOARD_SIZE][BOARD_SIZE];
    int height;
    int width;
} Board;

Board *create_starting_board(void);
Board *create_empty_board(void);

Piece *get_piece_at(const Board *board, Square square);
bool set_piece_at(Board *board, Square square, Piece *piece);
bool replace_piece_at(Board *board, Square square, Piece *piece);
bool destroy_piece_at(Board *board, Square square); // handles logic for removing a piece safely and destroys/frees the piece

bool destroy_board(Board *board);

bool is_square_on_board(const Board *board, Square square);
bool has_piece_at(const Board *board, Square square);
bool in_bounds(const Board *board, int row, int col);

bool clear_board(Board *board);

#endif