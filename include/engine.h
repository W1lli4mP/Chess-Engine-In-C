#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>

#include "game_state.h"
#include "move.h"

bool engine_find_best_move(GameState *game, int depth, Move *best_move_out);

#endif