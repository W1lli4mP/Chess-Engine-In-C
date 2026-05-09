#include <limits.h>

#include "engine.h"
#include "evaluation.h"
#include "move_gen.h"
#include "move_apply.h"
#include "rules.h"

#define CHECKMATE_SCORE 1000000

static int side_multiplier(Colour colour);

static int negamax(GameState *game, int depth, int alpha, int beta, bool *ok_out);

static int side_multiplier(Colour colour)
{
    if (colour == COLOUR_WHITE) return 1; 
    if (colour == COLOUR_BLACK) return -1;
    return 0;
}

// basically minimax but programmatically easier
// ok_out is used to track errors, since the return value is the score
static int negamax(GameState *game, int depth, int alpha, int beta, bool *ok_out)
{
    if (!game || !ok_out)
    {
        if (ok_out) *ok_out = false;
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
            // encourages faster checkmates
            return -CHECKMATE_SCORE + depth;
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

        // invert alpha and beta when switching sides for negamax
        int score = -negamax(game, depth - 1, -beta, -alpha, ok_out);

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

    // evaluate every position after a move is played
    for (int i = 0; i < moves.count; i++)
    {
        Move move = moves.moves[i];
        UndoInfo undo;

        // extract the evaluation after a move is played
        // then restore the game state to before the move is played
        if (!make_move(game, move, &undo)) return false;

        int score = -negamax(game, depth - 1, -beta, -alpha, &ok);

        if (!unmake_move(game, move, &undo)) return false;
        if (!ok) return false;

        // supersede better score + move
        if (score > best_score)
        {
            best_score = score;
            best_move = move;
        }

        if (score > alpha)
        {
            alpha = score;
        }
    }

    *best_move_out = best_move;
    return true;
}