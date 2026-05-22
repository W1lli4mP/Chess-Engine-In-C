#ifndef UCI_H
#define UCI_H

#include "game_state.h"
#include "move.h"

bool parse_uci_move(GameState *game, const char *text, Move *move_out);
bool move_to_uci(Move move, char out[6]);

#endif