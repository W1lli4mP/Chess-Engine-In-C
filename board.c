#include "board.h"
#include "move.h"
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
                        type = 'k';
                        break;
                    case 4:
                        type = 'q';
                        break;
                }
            }

            // calculate sprite
            const char *sprite = "-"; // if there is no piece, leave it's sprite as "--"
            switch (type) {
                case 'p':
                    sprite = (colour == 'w') ? "♟" : "♙";
                    break;
                case 'r':
                    sprite = (colour == 'w') ? "♜" : "♖";
                    break;
                case 'b':
                    sprite = (colour == 'w') ? "♝" : "♗";
                    break;
                case 'n':
                    sprite = (colour == 'w') ? "♞" : "♘";
                    break;
                case 'q':
                    sprite = (colour == 'w') ? "♛" : "♕";
                    break;
                case 'k':
                    sprite = (colour == 'w') ? "♚" : "♔";
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

Piece *get_piece_at(Board *board, int position[2]) { // TODO: add input validation
    return &board->grid[position[0]][position[1]];
}

void move_piece(Board *board, Move move) {
    // move selected piece to the destination square
    board->grid[move.to_row][move.to_col] = *move.piece;

    // remove old piece at the old square
    int old_position[2] = {move.from_row, move.from_col};
    remove_piece_at(board, old_position);
}

void remove_piece_at(Board *board, int position[2]) {
    board->grid[position[0]][position[1]].colour = '\0'; // reset the colour
    board->grid[position[0]][position[1]].type = '\0'; // reset the type
    board->grid[position[0]][position[1]].sprite = "-"; // reset the sprite
}

void make_move(Board *board, char *from_str, char *to_str) {
    // get coords from source square
    Position from_coords = square_to_coord(from_str);
    int from_square[] = {from_coords.row, from_coords.col};
    
    // get coords from destination square
    Position to_coords = square_to_coord(to_str);
    int to_square[] = {to_coords.row, to_coords.col};

    // create piece pointer for referencing in move struct
    Piece *selected_piece = get_piece_at(board, from_square);
    Move move = {selected_piece, from_square[0], from_square[1], to_square[0], to_square[1]};

    // execute the move
    move_piece(board, move);
}