#ifndef SQUARE_H
#define SQUARE_H

#include <stdbool.h>

// zero-indexed
typedef struct
{
    int row;
    int col;
} Square;

bool squares_equal(Square a, Square b);

#endif