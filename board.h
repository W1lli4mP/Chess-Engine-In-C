#ifndef BOARD_H
#define BOARD_H
#include "piece.h"
#include "position.h"
#include "move.h"
#include <stdbool.h>

typedef struct
{
    Piece *grid[8][8];
    int height;
    int width;
} Board;

Board *initialise_board();
Piece *get_piece_at(const Board *board, Position piece_pos);
bool set_piece_at(Board *board, Position piece_pos, Piece *piece);
bool remove_piece_at(Board *board, Position piece_pos); // handles logic for removing a piece safely and destroys/frees the piece
bool destroy_board(Board *board);
bool apply_move(Board *board, Move move);
bool valid_move(Board *board, Move move);
void print_board(const Board *board, int white_pov);
bool in_bounds(const Board *board, int row, int col);

#endif