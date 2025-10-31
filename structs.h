#ifndef STRUCTS_H
#define STRUCTS_H

// shared structs used across the project (made to solve errors caused by circular importing)

// position
typedef struct {
    int row;
    int col;
} Position;

// piece
typedef struct {
    char colour;
    char type;
    int position[2];
    const char *sprite;
} Piece;

// board
typedef struct {
    Piece grid[8][8]; // declares a grid of piece structs
} Board;
#include "move.h"

// move
typedef struct {
    Piece *piece;

    int from_row;
    int from_col;

    int to_row;
    int to_col;
} Move;

#endif