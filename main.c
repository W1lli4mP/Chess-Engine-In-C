#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "board.h"
#include "piece.h"
#include "move.h"

// declare functions
void process_input(Board *board, char[], char[]);

int main(void) {
    Board chess_board = init_pieces();

    bool running = true;
    char m1[] = "e2", m2[] = "e4";

    while (running) {
        process_input(&chess_board, m1, m2);
        make_move(&chess_board, m1, m2);
        display_grid(&chess_board);
    }

    return 0;
}

// define functions
void process_input(Board *board, char m1[], char m2[]) {
    // char move[6]; // longest input could be a 6-char pawn capture promotion (e.g. fxg8=Q)
    // need to add a function that reads the move (an algebraic move parser)
    printf("Enter a move: ");
    if (scanf("%2s %2s", m1, m2) != 2) {
        fprintf(stderr, "invalid input");
        exit(1);
    }
}