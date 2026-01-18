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
    PieceType promotion; // what the piece has promoted to
    bool is_capture;
    bool is_castle_kingside;
    bool is_castle_queenside;
    bool is_check;
    bool is_checkmate;
    bool is_promotion;
} Move;

Move *initialise_move();
bool destroy_move(Move *move);

#endif