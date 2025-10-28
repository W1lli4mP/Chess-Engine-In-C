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
Piece get_piece_at(Board, int[2]);
void move_piece(Board*, Piece*, int[2]);
void remove_piece(Board*, Piece *);

int main(void) {
    Board chess_board = init_pieces(chess_board);

    display_grid(chess_board.grid); // display chess board at the start before taking inputs
    
    // testing new functions
    // initialise square coords - could alternatively use int[2]{1, 1}
    int test_move_init[2] = {1, 1};
    int test_move[2] = {3, 1};
    Piece selected_piece = get_piece_at(chess_board, test_move_init);
    move_piece(&chess_board, &selected_piece, test_move);

    display_grid(chess_board.grid);

    // while (true) {
    //     process_input(chess_board.grid);
    // }

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

void process_input(Piece grid[ROWS][COLS]) {
    char move[6]; // longest input could be a 6-char pawn capture promotion (e.g. fxg8=Q)
    // need to add a function that reads the move (an algebraic move parser)
    printf("Enter a move: ");
    scanf("%s", move);
    display_grid(grid);
}

Piece get_piece_at(Board chess_board, int position[2]) {
    return chess_board.grid[position[0]][position[1]];
}

void move_piece(Board *board_pointer, Piece *piece_pointer, int move[2]) { // pointer to chess_board
    int destination_square[2] = {move[0], move[1]};
    
    remove_piece(board_pointer, piece_pointer);
    // use -> to modify real board rather than creating a local copy
    board_pointer->grid[destination_square[0]][destination_square[1]] = *piece_pointer;

    // future update: update piece position too
    piece_pointer->position[0] = destination_square[0];
    piece_pointer->position[1] = destination_square[1];
}

void remove_piece(Board *board_pointer, Piece *piece_pointer) {
    int original_square[2] = {piece_pointer->position[0], piece_pointer->position[1]};
    board_pointer->grid[original_square[0]][original_square[1]].colour = '\0'; // reset the colour
    board_pointer->grid[original_square[0]][original_square[1]].type = '\0'; // reset the type
    board_pointer->grid[original_square[0]][original_square[1]].sprite = "-"; // reset the sprite
}