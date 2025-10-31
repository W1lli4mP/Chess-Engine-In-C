#include <stdio.h>
#include <stdlib.h>
#include "move.h"

Position square_to_coord(char *square) {
    char col = square[0];
    char row = square[1];
    if (col < 97 || col > 104 || row < 48 || row > 56)  {// if row not in [a-h], col in not in [1-8]
        perror("invalid square");
        exit(1);
    }

    int x = row - 49; // added -1 since the board is 0-indexed
    int y = col - 97;
    Position coordinate = {x, y};
    return coordinate;
}
