#include <stdio.h>
#include <stdlib.h>

// config
#define COLS 8
#define ROWS 8

// define structs
struct piece {
    char colour;
    char type;
    int position[2];
    const char *sprite;
};

struct board {
    struct piece grid[ROWS][COLS]; // declares a grid of piece structs
};

// declare functions
void display_grid(struct piece grid[ROWS][COLS]); // expects a grid array of size ROWSxCOLS that stores piece structs
struct board init_pieces(struct board);


int main(void) {
    struct board chess_board = {0}; // creates an empty structure (board) called chess_board, {0} forces zero-initialisation to its variables

    chess_board = init_pieces(chess_board);
    display_grid(chess_board.grid);

    return 0;
}

// define functions
void display_grid(struct piece grid[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            struct piece p = grid[i][j];
            printf("%s ", p.sprite); // actually outputs the contents of each square
        }
        printf("\n");
    }
}

struct board init_pieces(struct board chess_board) { // returns an updated chess board full of initialised pieces
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            // create/declare piece struct
            struct piece current_piece = {0};

            // calculate piece's colour (using a ternary operator)
            char colour = (i == 0 || i == 1) ? 'w' : 'b';

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
            
            current_piece.colour = colour;
            current_piece.type = type;
            current_piece.position[0] = i; // cannot assign arrays directly, thus assign each element individually
            current_piece.position[1] = j;
            current_piece.sprite = sprite;

            // insert current piece struct into board struct to store it
            chess_board.grid[i][j] = current_piece;
        }
    }
    return chess_board;

}