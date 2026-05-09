#ifndef POSITION_KEY_H
#define POSITION_KEY_H

#include <stdbool.h>

#include "game_state.h"

#define POSITION_KEY_LEN 128

// compact form of a chess position to be used as a "lookup key"
// useful for threefold and fetching transpositions
typedef struct
{
    char text[POSITION_KEY_LEN];
} PositionKey;

bool create_position_key(const GameState *game, PositionKey *key_out);
bool position_keys_equal(const PositionKey *a, PositionKey *b);

#endif