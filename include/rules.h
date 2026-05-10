#ifndef RULES_H
#define RULES_H

#include <stdbool.h>
#include "game_state.h"

#include "square.h"

bool is_square_attacked(const GameState *game, Square square, Colour friendly_colour);
bool is_in_check(const GameState *game, Colour colour);

bool is_checkmate(const GameState *game, Colour colour);
bool is_stalemate(const GameState *game, Colour colour);

#endif