#include "board.h"
#include <stdio.h>

Board init_pieces(void) { // returns a newly initialised chess board with pieces
    Board chess_board;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            // calculate piece's colour (using a ternary operator)
            char colour = '\0'; // default if not assigned a value later on
            colour = (i == 0 || i == 1) ? 'w' : 'b';

            // calculate piece types
            char type = '\0'; // if there is no piece, leave it's type as '\0'
            // pawn row
            if (i == 1 || i == ROWS - 2) {
                type = 'p';
            // back row
            } else  if (i == 0 || i == ROWS - 1) {
                switch (j) {
                    case 0:
                    case COLS - 1:
                        type = 'r';
                        break;
                    case 1:
                    case COLS - 2:
                        type = 'n';
                        break;
                    case 2:
                    case COLS - 3:
                        type = 'b';
                        break;
                    case 3:
                        type = 'q';
                        break;
                    case 4:
                        type = 'k';
                        break;
                }
            }

            // calculate sprite
            const char *sprite = "-"; // if there is no piece, leave it's sprite as "--"
            switch (type) {
                case 'p':
                    sprite = (colour == 'w') ? "♙" : "♟";
                    break;
                case 'r':
                    sprite = (colour == 'w') ? "♖" : "♜";
                    break;
                case 'b':
                    sprite = (colour == 'w') ? "♗" : "♝";
                    break;
                case 'n':
                    sprite = (colour == 'w') ? "♘" : "♞";
                    break;
                case 'q':
                    sprite = (colour == 'w') ? "♕" : "♛";
                    break;
                case 'k':
                    sprite = (colour == 'w') ? "♔" : "♚";
                    break;
            }

            // declare and define piece struct
            Piece current_piece = {
                .colour = colour,
                .type = type,
                .position = {i, j},
                .sprite = sprite
            };

            // insert current piece struct into board struct to store it
            chess_board.grid[i][j] = current_piece;
        }
    }
    return chess_board;

}

void display_grid(Board *board) { // now takes in a pointer
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            Piece *p = &board->grid[i][j];
            printf("%s ", p->sprite); // actually outputs the contents of each square
        }
        printf("\n");
    }
}

Piece *get_piece_at(Board *chess_board, int position[2]) { // TODO: add input validation
    return &chess_board->grid[position[0]][position[1]];
}

void move_piece(Board *board_pointer, Piece *piece_pointer, int move[2]) { // pointer to chess_board
    int destination_square[2] = {move[0], move[1]};
    int original_square[2] = {piece_pointer->position[0], piece_pointer->position[1]};

    // make a local copy before deletion
    Piece selected_piece = *piece_pointer; 

    // update piece position
    selected_piece.position[0] = destination_square[0];
    selected_piece.position[1] = destination_square[1];

    // remove piece from original square
    remove_piece_at(board_pointer, original_square);

    // place copy onto the destination square
    board_pointer->grid[destination_square[0]][destination_square[1]] = selected_piece;

}

void remove_piece_at(Board *board_pointer, int position[2]) {
    board_pointer->grid[position[0]][position[1]].colour = '\0'; // reset the colour
    board_pointer->grid[position[0]][position[1]].type = '\0'; // reset the type
    board_pointer->grid[position[0]][position[1]].sprite = "-"; // reset the sprite
}