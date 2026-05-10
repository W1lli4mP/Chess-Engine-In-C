#include "game_play.h"
#include "move_apply.h"
#include "position_key.h"
#include "position_key_types.h"

bool play_move(GameState *game, Move move)
{
    if (!game) return false;

    UndoInfo undo;
    if (!make_move(game, move, &undo))
        return false;
    
    // append new real game position to repetition history
    if (!push_current_position(game))
    {
        // restore state if the position key failed to be pushed
        unmake_move(game, move, &undo);
        return false;
    }

    return true;
}

// retrieves the position key from the current game state's position
// then updates the game's position history
bool push_current_position(GameState *game)
{
    if (!game) return false;

    if (game->position_history_count >= MAX_POSITION_HISTORY) return false;

    PositionKey key;

    if (!create_position_key(game, &key)) return false;

    // add new position key to position history
    game->position_history[game->position_history_count] = key;
    game->position_history_count++;

    return true;
}