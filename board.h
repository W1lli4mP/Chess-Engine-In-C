#ifndef BOARD_H
#define BOARD_H

#include "piece.h"
#include "move.h"

#define ROWS 8
#define COLS 8

typedef struct {
    Piece grid[ROWS][COLS]; // declares a grid of piece structs
} Board;

Board init_pieces();
void display_grid(Board*, int);
Piece *get_piece_at(Board*, int[2]);
void move_piece(Board*, Move);
void remove_piece_at(Board*, int[2]);
void make_move(Board*, char*, char*);

#endif