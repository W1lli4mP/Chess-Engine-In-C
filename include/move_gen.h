#ifndef MOVE_GEN_H
#define MOVE_GEN_H

#include <stdbool.h>
#include "game_state.h"
#include "position.h"
#include "piece.h"
#include "board.h"

bool generate_pseudo_legal_moves(
    const Board *board,
    Position piece_location,
    PositionList *position_list_out
);

bool generate_legal_moves(const GameState *game,
    Position piece_location,
    PositionList *position_list_out
);

#endif