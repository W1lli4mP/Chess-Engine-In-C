#ifndef DRAW_RULES_H
#define DRAW_RULES_H

#include <stdbool.h>

#include "game_state.h"

bool is_fifty_move_draw(const GameState *game);
bool is_insufficient_material(const GameState *game);
bool is_threefold_repetition(const GameState *game);
bool is_draw(const GameState *game);

#endif