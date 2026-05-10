#include <limits.h>

#include "engine.h"
#include "evaluation.h"
#include "move_gen.h"
#include "move_apply.h"
#include "rules.h"
#include "draw_rules.h"
#include "position_key.h"

#ifdef ENGINE_DEBUG
#include <stdio.h>

#include "debug_print.h"
#endif

#define CHECKMATE_SCORE 1000000
#define MAX_SEARCH_HISTORY 256

static int side_multiplier(Colour colour);

// counts the current analysed position against both
//* - real positions already played in the game
//* - temporary positions reached in the current search line/branch

// lets the engine detect lines that reach threefold repetition
// without polluting game->position_history during the search
static bool key_repeats_threefold_in_search(
    const GameState *game,
    const PositionKey *search_history,
    int search_history_count
);

static int negamax(
    GameState *game,
    int depth,
    int alpha,
    int beta,
    PositionKey *search_history,
    int search_history_count,
    bool *ok_out
);

static int side_multiplier(Colour colour)
{
    if (colour == COLOUR_WHITE) return 1; 
    if (colour == COLOUR_BLACK) return -1;
    return 0;
}

static bool key_repeats_threefold_in_search(
    const GameState *game,
    const PositionKey *search_history,
    int search_history_count
)
{
    if (!game || !search_history || search_history_count <= 0)
        return false;

    const PositionKey *current = &search_history[search_history_count - 1];

    int count = 0;

    // real game history
    for (int i = 0; i < game->position_history_count; i++)
    {
        if (position_keys_equal(&game->position_history[i], current))
            count++;
    }

    // temporary search line history
    for (int i = 0; i < search_history_count; i++)
    {
        if (position_keys_equal(&search_history[i], current))
            count++;
    }

    return count >= 3;
}

// basically minimax but programmatically easier
// ok_out is used to track errors, since the return value is the score
static int negamax(
    GameState *game,
    int depth,
    int alpha,
    int beta,
    PositionKey *search_history,
    int search_history_count,
    bool *ok_out
)
{
    if (!game || !ok_out)
    {
        if (ok_out) *ok_out = false;
        return 0;
    }

    // drawn positions are neutral
    if (key_repeats_threefold_in_search(game, search_history, search_history_count))
    {
        return 0;
    }

    if (is_fifty_move_draw(game) || is_insufficient_material(game))
    {
        return 0;
    }

    // base case
    if (depth == 0)
    {
        return side_multiplier(game->side_to_move) * evaluate_position(game);
    }

    MoveList moves = {0};
    if (!generate_all_legal_moves(game, &moves))
    {
        *ok_out = false;
        return 0;
    }

    //* edge cases for stalemate/checkmate
    if (moves.count == 0)
    {
        if (is_in_check(game, game->side_to_move))
        {
            // current side to move is checkmated
            // subtracting depth makes faster checkmates better after negamax negates the score
            return -CHECKMATE_SCORE - depth;
        }

        // stalemate
        return 0;
    }

    int best_score = INT_MIN + 1;

    // simulate all moves
    for (int i = 0; i < moves.count; i++)
    {
        Move move = moves.moves[i];
        UndoInfo undo;

        //* make the move, calculate the score, unmake the move
        if (!make_move(game, move, &undo))
        {
            *ok_out = false;
            return 0;
        }

        PositionKey key;

        if (search_history_count >= MAX_SEARCH_HISTORY)
        {
            *ok_out = false;
            unmake_move(game, move, &undo);
            return 0;
        }

        if (!create_position_key(game, &key))
        {
            *ok_out = false;
            unmake_move(game, move, &undo);
            return 0;
        }

        // track the future position as part of the current analysed line
        search_history[search_history_count] = key;

        // invert alpha and beta when switching sides for negamax
        // and increment search history count
        int score = -negamax(
            game,
            depth - 1,
            -beta,
            -alpha,
            search_history,
            search_history_count + 1,
            ok_out
        );

        if (!unmake_move(game, move, &undo))
        {
            *ok_out = false;
            return 0;
        }

        //! do not continue further if failure happened in a previous recursive call
        if (!*ok_out) return 0;

        // update score
        if (score > best_score) best_score = score;

        // alpha-beta pruning
        if (score > alpha) alpha = score;

        if (alpha >= beta) break;
    }

    return best_score;
}

bool engine_find_best_move(GameState *game, int depth, Move *best_move_out)
{
    if (!game || !best_move_out || depth < 1) return false;

    // generate all legal moves
    MoveList moves = {0};

    if (!generate_all_legal_moves(game, &moves)) return false;
    if (moves.count == 0) return false;

    bool ok = true;
    int alpha = INT_MIN + 1;
    int beta = INT_MAX;

    int best_score = INT_MIN + 1;
    Move best_move = moves.moves[0];
    bool best_causes_threefold = false;

    // store temporary positions in searches to detect threefold repetition
    // allows future positions to be tracked within each branch/line in the search
    PositionKey search_history[MAX_SEARCH_HISTORY];

#ifdef ENGINE_DEBUG
    printf("[ENGINE] searching depth %d, legal moves: %d\n", depth, moves.count);
#endif

    // evaluate every position after a move is played
    for (int i = 0; i < moves.count; i++)
    {
        ok = true;

        Move move = moves.moves[i];
        UndoInfo undo;

        // extract the evaluation after a move is played
        // then restore the game state to before the move is played
        if (!make_move(game, move, &undo)) return false;

        PositionKey key;

        if (!create_position_key(game, &key))
        {
            unmake_move(game, move, &undo);
            return false;
        }

        search_history[0] = key;

        // check whether playing this move would immediately repeat the position for the third time
        bool causes_threefold = key_repeats_threefold_in_search(game, search_history, 1);

        int score;

        if (causes_threefold)
            score = 0;
        else
            score = -negamax(
                game,
                depth - 1,
                -beta,
                -alpha,
                search_history,
                1,
                &ok
            );

        if (!unmake_move(game, move, &undo)) return false;
        if (!ok) return false;

#ifdef ENGINE_DEBUG
        debug_print_engine_move_score(move, score);
        if (causes_threefold)
            puts("[ENGINE] move causes threefold repetition");
#endif

        // supersede better score + move
        if (
            score > best_score ||
            (score == best_score && best_causes_threefold && !causes_threefold)
        )
        {
            best_score = score;
            best_move = move;
            best_causes_threefold = causes_threefold;
        }

        if (score > alpha)
        {
            alpha = score;
        }
    }

#ifdef ENGINE_DEBUG
    debug_print_engine_best_move(best_move, best_score);
#endif

    *best_move_out = best_move;
    return true;
}