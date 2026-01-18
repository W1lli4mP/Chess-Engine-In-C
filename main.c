#include <stdio.h>
#include "piece.h"
#include "board.h"
#include "position.h"
#include "parser.h"
#include "move.h"

static char piece_type_to_char(PieceType p);

int main(int argc, char **argv)
{
    // print piece
    // Board *board = initialise_board();
    // print_board(board, 0); // 0 = black view, 1 = white view

    // Move move = { .from.col = 0, .from.row = 0, .to.col = 1, .to.row = 1 };
    // apply_move(board, move);

    // printf("------------------------\n");
    // print_board(board, 0);

    // destroy_board(board);

    /*
        TODO
        - add more piece movement validation (legal moves)
        - add algebraic chess notation
        - add user inputs
        - add game loop
        - extend game mechanics
            - checks and checkmate
            - stalemate
            - en passant
            - castling
        - add user inputs
            - algebraic chess parser
    */

    // parser testing
    if (argc != 2) return 1;
    const char *input = argv[1];
    int err_pos = 0;
    Move *move = algebraic_chess_parser(input, &err_pos);

    printf("Input: \"%s\", err: %d\n", input, err_pos);

    // error handling
    if (!move)
    {
        printf("Parse failed at %d\n", err_pos);
        return 1;
    }

    printf("%c, (%d, %d) to (%d, %d)\n", piece_type_to_char(move->piece), move->from.col, move->from.row, move->to.col, move->to.row);
    if (move->is_castle_kingside || move->is_castle_queenside) printf("Castling\n");
    if (move->is_check) printf("Check!\n");
    if (move->is_checkmate) printf("Checkmate!\n");
    if (move->is_promotion) printf("Promoting to %c\n", piece_type_to_char(move->promotion));
    destroy_move(move);

    return 0;
}

static char piece_type_to_char(PieceType p)
{
    switch (p)
    {
        case TYPE_ROOK: return 'R';
        case TYPE_KNIGHT: return 'N';
        case TYPE_BISHOP: return 'B';
        case TYPE_QUEEN: return 'Q';
        case TYPE_KING: return 'K';
        case TYPE_PAWN: return 'P';
        default: return '-';
    }
}

// where i left off: fix parser disambiguation moves and captures