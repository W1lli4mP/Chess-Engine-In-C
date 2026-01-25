#ifndef MOVE_GEN_H
#define MOVE_GEN_H
#include "position.h"
#include "piece.h"
#include "board.h"
#include "rules.h"
#include <stdbool.h>

#define MAX_MOVES 256

typedef struct
{
    Position moves[MAX_MOVES];
    int count;
} PositionList;

bool generate_pseudo_legal_moves(const Board *board, Position piece_location, PositionList *position_list_out);

// main move generator
bool generate_legal_moves(const Board *board, Position piece_location, PositionList *position_list_out);
#endif