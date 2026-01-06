#include <stdio.h>
#include "piece.h"
#include "board.h"
#include "position.h"

int main()
{
    // print piece
    Board *board = initialise_board();
    print_board(board, 0); // 0 = black view, 1 = white view

    Move move = { .from.col = 0, .from.row = 0, .to.col = 1, .to.row = 1 };
    apply_move(board, move);

    printf("------------------------\n");
    print_board(board, 0);

    destroy_board(board);

    /*
        TODO
        - add more piece movement validation (legal moves)
        - add algebraic chess notation
        - add user inputs
        - add game loop
        - extend game mechanics
            - checks and checkmate
            - stalemate
            - en passant
            - castling
        - add user inputs
            - algebraic chess parser
    */

    return 0;
}