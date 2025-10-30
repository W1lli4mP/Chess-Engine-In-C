#ifndef MOVE_H
#define MOVE_H

#include "piece.h"

typedef struct {
    Piece *piece;

    int from_row;
    int from_col;

    int to_row;
    int to_col;
} Move;

typedef struct {
    int row;
    int col;
} Position;

// basic helper function
Position square_to_coord(char*);

#endif