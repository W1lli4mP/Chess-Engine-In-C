#include "position.h"

bool position_list_append(PositionList *position_list, Position move)
{
    if (position_list->count >= MAX_MOVES) return false;
    position_list->moves[position_list->count++] = move;
    return true;
}