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

static void san_debug(const San san)
{
    printf("[SAN] Attempting to move %c to %c%c\n", piece_type_to_char(san.piece), san.to.col + 'a', san.to.row + '1');
}

static void move_debug(const Move move)
{
    printf("[MOVE] Attempting to move %c from %c%c to %c%c\n", piece_type_to_char(move.piece), move.from.col + 'a', move.from.row + '1', move.to.col + 'a', move.to.row + '1');
}

static void san_resolver_debug(const ResolveStatus status)
{
    if (status == RESOLVE_OK) puts("[SAN RESOLVER] LEGAL MOVE!");
    if (status == RESOLVE_AMBIGUOUS) puts("[SAN RESOLVER] AMBIGUOUS MOVE!");
    if (status == RESOLVE_ILLEGAL) puts("[SAN RESOLVER] ILLEGAL MOVE!");
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

        // 3) convert input to SAN (string -> san)
        int err_pos = -1;
        San *san = algebraic_chess_parser(input, &err_pos);

        // error handling
        if (!san)
        {
            fprintf(stderr, "Parsing failed at %d\n", err_pos);
            return 1;
        }

        // SAN DEBUG PRINT
        san_debug(*san);

        // 4) resolve SAN (san -> move)
        Move *move = initialise_move();
        
        ResolveStatus status = resolve_san(board, *san, move);
        destroy_san(san);

        // SAN RESOLVER DEBUG PRINT
        san_resolver_debug(status);

        // only allow OK moves to be played
        if (status != RESOLVE_OK)
        {
            destroy_move(move);
            continue;
        }

        // MOVE DEBUG PRINT
        move_debug(*move);

        if (!apply_move(board, *move)) puts("Move failed!");

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