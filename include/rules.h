#ifndef RULES_H
#define RULES_H
#include "position.h"
#include "piece.h"
#include "board.h"
#include "move_gen.h"
#include "position.h"
#include <stdbool.h>

bool is_square_attacked(const Board *board, Position square, Colour friendly_colour);
bool is_checkmate(const Board *board, Colour colour);
bool is_stalemate(const Board *board, Colour colour);
bool generate_legal_moves(const Board *board, Position piece_location, PositionList *position_list_out);

#endif