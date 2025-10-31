#include <stdio.h>
#include <stdbool.h>
#include "board.h"
#include "piece.h"
#include "move.h"

// declare functions
void process_input(Board*);

int main(void) {
    Board chess_board = init_pieces();

    display_grid(&chess_board); // display chess board at the start before taking inputs

    make_move(&chess_board, "e2", "e4");

    display_grid(&chess_board);

    make_move(&chess_board, "e7", "e5");

    display_grid(&chess_board);

    return 0;
}

// define functions
void process_input(Board *board) {
    char move[6]; // longest input could be a 6-char pawn capture promotion (e.g. fxg8=Q)
    // need to add a function that reads the move (an algebraic move parser)
    printf("Enter a move: ");
    scanf("%s", move);
    display_grid(board);
}