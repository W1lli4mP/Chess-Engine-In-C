#include <stdio.h>
#include <stdbool.h>
#include "piece.h"
#include "board.h"
#include "position.h"
#include "parser.h"
#include "move.h"
#include "san.h"
#include "move_gen.h"
#include "san_resolve.h"

#define WHITE_VIEW 1
#define BLACK_VIEW 0

// helper for printing
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

int main()
{
    Board *board = initialise_board();

    bool running = true;
    while (running)
    {
        // 1) draw board
        print_board(board, WHITE_VIEW); // 0 = black view, 1 = white view

        // 2) ask for input
        char input[8];
        // infinite loop until input is valid
        while (1)
        {
            if (!fgets(input, sizeof input, stdin))
            {
                puts("Invalid input entered\n");
                return 1;
            }
            
            size_t n = strcspn(input, "\n");

            // terminate newline if newline found (implies input length is sufficient)
            if (input[n] == '\n')
            {
                input[n] = '\0';
                break;
            }

            int c;
            while ((c = getchar() != '\n' && c != EOF)) {} // no newline implies too many chars, flush remaining chars
            puts("Too long, try again");
        }

        // 3) apply the input (move)
        int err_pos = -1;
        San *san = algebraic_chess_parser(input, &err_pos);

        // error handling
        if (!san)
        {
            fprintf(stderr, "Parsing failed at %d\n", err_pos);
            return 1;
        }

        // SAN DEBUG PRINT
        printf("[SAN] Attempting to move %c to %c%c\n", piece_type_to_char(san->piece), san->to.col + 'a', san->to.row + '1');

        // TODO: add semantic analysis
        Move *move = initialise_move();
        
        puts("Resolving SAN");
        ResolveStatus status = resolve_san(board, *san, move);
        destroy_san(san);

        if (status == RESOLVE_ILLEGAL) puts("ILLEGAL MOVE!");
        if (status == RESOLVE_AMBIGUOUS) puts("AMBIGUOUS MOVE!");

        // MOVE DEBUG PRINT
        printf("[MOVE] Attempting to move %c from %c%c to %c%c\n", piece_type_to_char(move->piece), move->from.col + 'a', move->from.row + '1', move->to.col + 'a', move->to.row + '1');

        puts("Attempting to apply move");
        if (!apply_move(board, *move))
        {
            puts("Move failed!");
        }
        destroy_move(move);

        puts("------------------------");

    }

    destroy_board(board);

    
    return 0;
}

    /*
    game loop structure
    draw board
    ask input
    apply input
    
    draw board if input valid
    else retry input
    
    */

    /*
    DEBUG PRINTS

    printf("Input: \"%s\", err: %d\n", input, err_pos);
    printf("%c, (%d, %d) to (%d, %d)\n", piece_type_to_char(move->piece), move->from.col, move->from.row, move->to.col, move->to.row);
    if (move->is_castle_kingside || move->is_castle_queenside) printf("Castling\n");
    if (move->is_check) printf("Check!\n");
    if (move->is_checkmate) printf("Checkmate!\n");
    if (move->is_promotion) printf("Promoting to %c\n", piece_type_to_char(move->promotion));
    destroy_move(move);
    */


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