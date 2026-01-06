#ifndef BOARD_H
#define BOARD_H

#include "structs.h"

Board init_pieces();
void display_grid(Board*, int);
Piece *get_piece_at(Board *board, Position pos);
void move_piece(Board*, Move);
void remove_piece_at(Board*, int[2]);
void make_move(Board*, char*, char*);

#endif