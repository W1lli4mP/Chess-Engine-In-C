#ifndef GAME_PLAY_H
#define GAME_PLAY_H

#include <stdbool.h>

#include "game_state.h"
#include "move.h"

bool play_move(GameState *game, Move move);
bool push_current_position(GameState *game);

#endif