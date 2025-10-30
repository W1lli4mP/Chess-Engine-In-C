#include <stdio.h>
#include "board.h"
#include "piece.h"
#include "move.h"

// declare functions
void process_input(Board*);

int main(void) {
    Board chess_board = init_pieces();

    display_grid(&chess_board); // display chess board at the start before taking inputs
    
    // testing new functions
    Position from_coords = square_to_coord("e2");
    int from_square[] = {from_coords.row, from_coords.col};
    Position to_coords = square_to_coord("e4");
    int to_square[] = {to_coords.row, to_coords.col};

    Piece *selected_piece = get_piece_at(&chess_board, from_square); // piece pointer

    Move move = {selected_piece, from_square[0], from_square[1], to_square[0], to_square[1]};

    move_piece(&chess_board, move);

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