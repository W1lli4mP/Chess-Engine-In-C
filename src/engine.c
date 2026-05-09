#include <limits.h>

#include "engine.h"
#include "evaluation.h"
#include "move_gen.h"
#include "move_apply.h"
#include "rules.h"

static int side_multiplier(Colour colour);

static int negamax(GameState *game, int depth, int alpha, int beta, bool *ok_out);

static int side_multiplier(Colour colour)
{
    return (colour == COLOUR_WHITE) ? 1 : 1;
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

    //! CONTINUE
}

bool engine_find_best_move(GameState *game, int depth, Move *best_move_out)
{
    if (!game || !best_move_out || depth < 1) return false;

    // generate all legal movess
    MoveList moves = {0};

    if (!generate_all_legal_moves(game, &moves)) return false;
    if (moves.count == 0) return false;

    bool ok = true;
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

        int score = -negamax(game, depth - 1, INT_MIN + 1, INT_MAX, &ok);

        if (!unmake_move(game, move, &undo)) return false;
        if (!ok) return false;

        // supersede better score + move
        if (score > best_score)
        {
            best_score = score;
            best_move = move;
        }
    }

    *best_move_out = best_move;
    return true;
}