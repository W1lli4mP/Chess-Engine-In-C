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

    bool is_en_passant;

    // promotion
    bool is_promotion;
    PieceType promotion;
} Move;

typedef struct
{
    Move moves[MAX_MOVES];
    int count;
} MoveList;

Move create_move(PieceType piece, Position from, Position to);
bool move_list_append(MoveList *move_list, Move move);

Move *initialise_move();
bool destroy_move(Move *move);

#endif