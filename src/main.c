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
#include "fen_parser.h"
#include "game_state.h"
#include "rules.h"
#include "move_apply.h"
#include "debug_print.h"
#include "engine.h"

#define WHITE_VIEW 1
#define BLACK_VIEW 0

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

static bool is_game_over(const GameState *game)
{
    if (is_checkmate(game, COLOUR_WHITE))
    {
        puts("[GAME] Black wins!");
        return true;
    }
    else if (is_checkmate(game, COLOUR_BLACK))
    {
        puts("[GAME] White wins!");
        return true;
    }
    return false;
}

static void turn_debug(Colour side_to_move)
{
    if (side_to_move == COLOUR_WHITE) puts("[GAME] White to move");
    if (side_to_move == COLOUR_BLACK) puts("[GAME] Black to move");
}

int main()
{
    GameState *game = create_starting_game_state();

    bool running = true;
    while (running)
    {
        // 1) indicate turn
        turn_debug(game->side_to_move);

        // 2) draw board
        debug_print_board(game->board, WHITE_VIEW); // 0 = black view, 1 = white view

        // 3) check game conditions
        if (is_game_over(game))
        {
            running = false;
            break;
        }

        // 4) ask for input
        //* human input
        if (game->side_to_move == COLOUR_WHITE)
        {
            char input[8];
            if (!ask_input(input, sizeof input)) return 1;

            // 5) convert input to SAN (string -> san)
            int err_pos = -1;
            San *san = algebraic_chess_parser(input, &err_pos);

            // error handling
            if (!san)
            {
                fprintf(stderr, "Parsing failed at %d\n", err_pos);
                return 1;
            }

            // SAN DEBUG PRINT
            debug_print_san(*san);

            // 6) resolve SAN (san -> move)
            Move move = {0};
            
            ResolveStatus status = resolve_san(game, *san, &move);
            destroy_san(san);

            // SAN RESOLVER DEBUG PRINT
            debug_print_resolve_status(status);

            // only allow OK moves to be played
            if (status != RESOLVE_OK) continue;

            // MOVE DEBUG PRINT
            debug_print_move(move);

            UndoInfo undo;

            // replace apply_move() with new make_move() system
            if (!make_move(game, move, &undo))
            {
                puts("Move failed!");
                continue;
            }
        }
        // AI input
        else if (game->side_to_move == COLOUR_BLACK)
        {
            int search_depth = 2;

            Move ai_move = {0};
            engine_find_best_move(game, search_depth, &ai_move);

            UndoInfo undo;
            if (!make_move(game, ai_move, &undo))
            {
                puts("Move failed!");
                continue;
            }
        }

        puts("------------------------");
    }

    destroy_game_state(game);
    
    return 0;
}

    /*
        game loop structure
        draw board
        ask input (human or AI)
        apply input
    
        draw board if input valid
        else retry input
    
    */

    /*
        TODO
        - improve:
            - searching
            - evaluating
                - import NNUE
        
        IT WORKS
    */