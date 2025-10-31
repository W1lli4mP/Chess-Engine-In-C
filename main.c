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
    char m1[] = "e2", m2[] = "e4"; // initial values

    display_grid(&chess_board, 0);
    while (running) {
        process_input(&chess_board, m1, m2); // modifies m1 and m2
        make_move(&chess_board, m1, m2);
        display_grid(&chess_board, 0);
    }

    return 0;
}

// define functions
void process_input(Board *board, char m1[], char m2[]) {
    // temporarily reads squares only - no SAN logic involved
    printf("Enter a move: ");
    if (scanf("%2s %2s", m1, m2) != 2) {
        fprintf(stderr, "invalid input");
        exit(1);
    }
}