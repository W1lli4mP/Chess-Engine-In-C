#ifndef PIECE_H // define if not defined yet - include guard
#define PIECE_H

#include "structs.h"
#include "board.h"

// main move generator
int generate_pseudo_legal_moves(Board *b, Piece *p, Position *moves);

// sub generators
int generate_pawn_moves(Board *b, Piece *p, Position *moves, int capacity);
// void generate_sliding_moves(void); // helper to simpify logic
// void generate_knight_moves(void);
// void generate_king_moves(void);

// helper functions
int is_enemy(Board *board, Piece *piece, Position destination);
int in_bounds(int row, int col);

#endif