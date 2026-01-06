#ifndef BOARD_H
#define BOARD_H
#include "piece.h"
#include "position.h"
#include <stdbool.h>

typedef struct
{
    Piece *grid[8][8];
    int height;
    int width;
} Board;

Board *initialise_board();
Piece *get_piece_at(Board *board, Position piece_pos);
bool set_piece_at(Board *board, Position piece_pos, Piece *piece);
bool remove_piece_at(Board *board, Position piece_pos); // handles logic for removing a piece safely and destroys/frees the piece
bool destroy_board(Board *board);
void print_board(Board *board, int white_pov);

#endif