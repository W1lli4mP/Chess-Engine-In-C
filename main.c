#include <stdio.h>
#include "piece.h"
#include "board.h"
#include "position.h"

int main()
{
    // print piece
    Board *board = initialise_board();
    print_board(board, 0); // 0 = black view, 1 = white view

    return 0;
}