#include <stdio.h>
#include "board.h"
#include "piece.h"

// declare functions
void process_input(Board*);

int main(void) {
    Board chess_board = init_pieces();

    display_grid(&chess_board); // display chess board at the start before taking inputs
    
    // testing new functions
    // initialise square coords - could alternatively use int[2]{1, 1}
    int test_move_init[2] = {1, 1};
    int test_move[2] = {3, 1};
    Piece *selected_piece = get_piece_at(&chess_board, test_move_init);
    move_piece(&chess_board, selected_piece, test_move);

    display_grid(&chess_board);

    // while (true) {
    //     process_input(chess_board.grid);
    // }

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