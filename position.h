#ifndef POSITION_H
#define POSITION_H

#define MAX_MOVES 256

// zero-indexed
typedef struct
{
    int row;
    int col;
} Position;

// list of positions
typedef struct PositionList
{
    Position moves[MAX_MOVES];
    int count;
} PositionList;

#endif