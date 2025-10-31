#ifndef PIECE_H // define if not defined yet - include guard
#define PIECE_H

#include "structs.h"
#include "board.h"

// main move generator
int generate_pseudo_legal_moves(Board *b, Piece *p, Position *moves);

// sub generators
int generate_pawn_moves(Board *b, Piece *p, Position *moves, int capacity);
int generate_knight_moves(Board *b, Piece *p, Position *moves, int capacity);
int generate_king_moves(Board *b, Piece *p, Position *moves, int capacity);
int generate_sliding_moves(Board *b, Piece *p, Position *moves, int capacity, int directions[][2], int num_d); // helper to simpify logic
int generate_bishop_moves(Board *b, Piece *p, Position *moves, int capacity);
int generate_rook_moves(Board *b, Piece *p, Position *moves, int capacity);
int generate_queen_moves(Board *b, Piece *p, Position *moves, int capacity);

// helper functions
int is_enemy(Board *board, Piece *piece, Position destination);
int in_bounds(int row, int col);

#endif