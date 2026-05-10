#ifndef FEN_PARSER_H
#define FEN_PARSER_H

#include <stdbool.h>
#include <ctype.h>

#include "game_state.h"

bool load_fen(GameState *game, const char *fen, int *err_pos);

#endif