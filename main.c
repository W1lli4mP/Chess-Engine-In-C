#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// config
#define COLS 8
#define ROWS 8

// define structs
typedef struct {
    char colour;
    char type;
    int position[2];
    const char *sprite;
} Piece;

typedef struct {
    Piece grid[ROWS][COLS]; // declares a grid of piece structs
} Board;

// declare functions
void display_grid(Piece grid[ROWS][COLS]); // expects a grid array of size ROWSxCOLS that stores piece structs
Board init_pieces(Board);
void process_input(Piece grid[ROWS][COLS]);

int main(void) {
    Board chess_board = init_pieces(chess_board);

    display_grid(chess_board.grid); // display chess board at the start before taking inputs
    while (true) {
        process_input(chess_board.grid);
    }

    return 0;
}

// define functions
void display_grid(Piece grid[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            Piece p = grid[i][j];
            printf("%s ", p.sprite); // actually outputs the contents of each square
        }
        printf("\n");
    }
}

Board init_pieces(Board chess_board) { // returns an updated chess board full of initialised pieces
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
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

void process_input(Piece grid[ROWS][COLS]) {
    char move[6]; // longest input could be a 6-char pawn capture promotion (e.g. fxg8=Q)
    // need to add a function that reads the move (an algebraic move parser)
    printf("Enter a move: ");
    scanf("%s", move);
    display_grid(grid);
}