#include <stdio.h>
#include <stdlib.h>

// config
#define COLS 8
#define ROWS 8

struct board {
    char *grid[ROWS][COLS];
};

int main(void) {
    struct board chessBoard = { // creates a structure variable (board) called chessBoard
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
    
    int count = 1;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%d\n", count++); // verifying if all 64 squares are printed out
            // printf("%s\n", chessBoard.grid[i][j]); // actually outputs the contents of each square
        }
    }

    return 0;
}