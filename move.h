#ifndef MOVE_H
#define MOVE_H
#include "position.h"
#include "piece.h"
#include <stdbool.h>
#include <stdlib.h>
typedef struct
{
    PieceType piece;
    Position from;
    Position to;
    bool is_capture;

    // castling
    bool is_castle_kingside;
    bool is_castle_queenside;

    // promotion
    bool is_promotion;
    PieceType promotion;

    // check/mate
    bool is_check;
    bool is_checkmate;
} Move;

Move *initialise_move();
bool destroy_move(Move *move);

#endif