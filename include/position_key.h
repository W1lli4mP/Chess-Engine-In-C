#ifndef POSITION_KEY_H
#define POSITION_KEY_H

#include <stdbool.h>

#include "game_state.h"
#include "position_key_types.h"

bool create_position_key(const GameState *game, PositionKey *key_out);
bool position_keys_equal(const PositionKey *a, const PositionKey *b);

#endif