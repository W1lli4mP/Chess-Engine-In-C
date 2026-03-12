#ifndef SAN_RESOLVE_H
#define SAN_RESOLVE_H
#include "san.h"
#include "move.h"
#include "move_gen.h"
#include "board.h"

typedef enum
{
    RESOLVE_OK,
    RESOLVE_ILLEGAL,
    RESOLVE_AMBIGUOUS
} ResolveStatus;


ResolveStatus resolve_san(const Board *board, San san, Colour side_to_move, Move *move_out);

#endif