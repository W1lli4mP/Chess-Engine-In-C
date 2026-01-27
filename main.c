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

static bool ask_input(char *input_out, size_t input_size)
{
    // infinite loop until input is valid
    for (;;)
    {
        if (!fgets(input_out, input_size, stdin))
        {
            puts("Invalid input entered\n");
            return false;
        }
        
        size_t n = strcspn(input_out, "\n");

        // terminate newline if newline found (implies input length is sufficient)
        if (input_out[n] == '\n')
        {
            input_out[n] = '\0';
            return true;
        }

        int c;
        while (((c = getchar()) != '\n' && c != EOF)) {} // no newline implies too many chars, flush remaining chars
        puts("Too long, try again");
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
        if (!ask_input(input, sizeof input)) return 1;

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

        Move *move = initialise_move();
        
        puts("Resolving SAN");
        ResolveStatus status = resolve_san(board, *san, move);
        destroy_san(san);

        if (status == RESOLVE_ILLEGAL) puts("ILLEGAL MOVE!");
        if (status == RESOLVE_AMBIGUOUS) puts("AMBIGUOUS MOVE!");

        // dont attempt to apply move if it is illegal
        if (status == RESOLVE_ILLEGAL)
        {
            destroy_move(move);
            continue;
        }

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
        TODO
        - add more piece movement validation (legal moves)
        - add game loop
            - add turn handling
        - extend game mechanics
            - checks and checkmate
            - stalemate
            - en passant
            - castling
        - finalise san resolver
            - use all san attributes
    */