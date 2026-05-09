#include "game_play.h"
#include "move_apply.h"

bool play_move(GameState *game, Move move)
{
    if (!game) return false;

    UndoInfo undo;
    if (!make_move(game, move, &undo))
        return false;
    
    // append to position history when implemented

    return true;
}