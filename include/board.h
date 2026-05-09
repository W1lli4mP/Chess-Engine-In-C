#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

#include "piece.h"
#include "position.h"
#include "move.h"

#define BOARD_SIZE 8

typedef struct
{
    Piece *grid[BOARD_SIZE][BOARD_SIZE];
    int height;
    int width;
} Board;

Board *create_starting_board(void);
Board *create_empty_board(void);

Piece *get_piece_at(const Board *board, Position piece_pos);
bool set_piece_at(Board *board, Position piece_pos, Piece *piece);
bool replace_piece_at(Board *board, Position piece_pos, Piece *piece);
bool destroy_piece_at(Board *board, Position piece_pos); // handles logic for removing a piece safely and destroys/frees the piece

bool destroy_board(Board *board);

bool is_move_on_board(const Board *board, Move move);
bool in_bounds(const Board *board, int row, int col);

bool clear_board(Board *board);

#endif