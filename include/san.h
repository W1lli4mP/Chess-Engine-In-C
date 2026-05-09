#ifndef SAN_H
#define SAN_H

#include <stdlib.h>
#include <stdbool.h>
#include "piece.h"
#include "position.h"

typedef enum
{
    SAN_SUFFIX_NONE,
    SAN_SUFFIX_CHECK,
    SAN_SUFFIX_MATE
} SanSuffix;

typedef struct
{
    PieceType piece;
    Position to;
    bool is_capture;

    // castling intent
    bool is_castle_kingside;
    bool is_castle_queenside;

    // disambiguation hints
    int from_col;
    int from_row;

    // promotion
    bool is_promotion;
    PieceType promotion;

    SanSuffix suffix;
} San;

San *create_san(void);
bool destroy_san(San *san);

#endif