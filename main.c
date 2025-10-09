#include <stdio.h>
#include <stdlib.h>

// config
#define COLS 8
#define ROWS 8

struct board {
    char *grid[ROWS][COLS];
};

void display_grid(char *grid[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%s\n", grid[i][j]); // actually outputs the contents of each square
        }
    }
}

int main(void) {
    struct board chess_board = { // creates a structure variable (board) called chess_board
        .grid = {
                {"br", "bn", "bb", "bq", "bk", "bb", "bn", "br"},
                {"bp", "bp", "bp", "bp", "bp", "bp", "bp", "bp"},
                {"--", "--", "--", "--", "--", "--", "--", "--"},
                {"--", "--", "--", "--", "--", "--", "--", "--"}, 
                {"--", "--", "--", "--", "--", "--", "--", "--"},
                {"--", "--", "--", "--", "--", "--", "--", "--"},
                {"wp", "wp", "wp", "wp", "wp", "wp", "wp", "wp"}, 
                {"wr", "wn", "wb", "wq", "wk", "wb", "wn", "wr"},
                }
    };
    
    // for (int i = 0; i < ROWS; i++) {
    //     for (int j = 0; j < COLS; j++) {
    //         printf("%s\n", chess_board.grid[i][j]); // actually outputs the contents of each square
    //     }
    // }
    display_grid(chess_board.grid);

    return 0;
}