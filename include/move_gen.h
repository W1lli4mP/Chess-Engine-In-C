#ifndef MOVE_GEN_H
#define MOVE_GEN_H

#include <stdbool.h>
#include "game_state.h"
#include "position.h"
#include "piece.h"
#include "board.h"
#include "move.h"

bool generate_pseudo_legal_moves(
    const GameState *game,
    Position piece_location,
    MoveList *moves_out
);

bool generate_legal_moves(
    const GameState *game,
    Position piece_location,
    MoveList *moves_out
);

#endif