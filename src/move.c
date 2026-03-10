#include "move.h"

// sets default values
Move *initialise_move()
{
    Move *move = malloc(sizeof *move);
    Position from = { .col = -1, .row = -1 };
    Position to = { .col = -1, .row = -1 };

    move->piece = TYPE_NONE;
    move->from = from;
    move->to = to;
    move->is_capture = false;

    move->is_castle_kingside = false;
    move->is_castle_queenside = false;

    move->is_promotion = false;
    move->promotion = TYPE_NONE;

    return move;
}

bool destroy_move(Move *move)
{
    if (!move) return false;

    free(move);

    return true;
}