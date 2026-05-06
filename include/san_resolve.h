#ifndef SAN_RESOLVE_H
#define SAN_RESOLVE_H
#include "san.h"
#include "move.h"
#include "game_state.h"

typedef enum
{
    RESOLVE_OK,
    RESOLVE_ILLEGAL,
    RESOLVE_AMBIGUOUS
} ResolveStatus;


ResolveStatus resolve_san(GameState *game, San san, Move *move_out);

#endif