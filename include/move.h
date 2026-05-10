#ifndef MOVE_H
#define MOVE_H

#include <stdbool.h>
#include <stdlib.h>

#include "square.h"
#include "piece.h"

#define MAX_MOVES 256

typedef struct
{
    PieceType piece;

    Square from;
    Square to;
    
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

Move create_move(PieceType piece, Square from, Square to);
bool move_list_append(MoveList *move_list, Move move);

#endif