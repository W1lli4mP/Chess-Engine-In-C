#include "square.h"

bool squares_equal(Square a, Square b)
{
    return a.row == b.row && a.col == b.col;
}