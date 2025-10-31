#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "board.h"
#include "piece.h"
#include "move.h"
#include "rules.h"

// declare functions
void process_input(Board*, char[], char[]);
int validate_move(Board*, char[], char[]);

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
    int valid = 0;
    // temporarily reads squares only - no SAN logic involved
    while (!valid) {
        printf("Enter a move: ");
        if (scanf("%2s %2s", m1, m2) != 2) {
            fprintf(stderr, "invalid input");
            exit(1);
        }
        valid = validate_move(board, m1 ,m2);
        if (!valid)
            printf("Invalid, try again\n");
}
}

int validate_move(Board *b, char m1[], char m2[]) {
    Position from_coords = square_to_coord(m1);
    int from_square[2] = {from_coords.row, from_coords.col};
    Position to_coords = square_to_coord(m2);
    int to_square[2] = {to_coords.row, to_coords.col};

    Piece *p = get_piece_at(b, from_square);
    Position moves[20];
    
    int num_moves = generate_pseudo_legal_moves(b, p, moves);
    
    Position out = {0, 0};
    if (get_king_position(b, 'w', &out))
        printf("WHITE KING FOUND AT: %d, %d\n", out.row, out.col);

    int valid = 0;

    for (int i = 0; i < num_moves; i++) {
        if (to_coords.row == moves[i].row && to_coords.col == moves[i].col) {
            return 1; // valid
        }
    }
    return 0; // invalid
}