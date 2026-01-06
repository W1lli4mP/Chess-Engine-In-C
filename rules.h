#ifndef RULES_H
#define RULES_H

#include "structs.h"
#include "move.h"
#include "board.h"
#include "piece.h"

#include <stdlib.h>

Position get_king_position(Board *b, char colour);

int is_in_check(Board *b, char colour);

int is_checkmate(Board *b, char colour);

// int has_any_legal_move(Board *b, char colour);
//     /* true if side has at least one legal move */

// int would_cause_check(Board *b, Move mv);
//     /* apply mv (temporarily) and return 1 if it leaves mover in check, 0 otherwise.
//        Implementation must be reversible (apply+undo) or work on a copy. */

// int generate_legal_moves_for_piece(Board *b, Piece *p, Position *out_moves, int capacity);
//     /* filters pseudo-legal moves and returns number of legal moves */

#endif
