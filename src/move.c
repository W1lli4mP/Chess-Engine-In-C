#include "move.h"

// creates a move without having to allocate to memory
Move create_move(PieceType piece, Position from, Position to)
{
    Move move = {
        .piece = piece,
        .from = from,
        .to = to,
        .is_capture = false,

        .is_castle_kingside = false,
        .is_castle_queenside = false,

        .is_en_passant = false,

        .is_promotion = false,
        .promotion = TYPE_NONE
    };

    return move;
}

bool move_list_append(MoveList *move_list, Move move)
{
    if (!move_list)
        return false;
    
    if (move_list->count >= MAX_MOVES)
        return false;
    
    move_list->moves[move_list->count] = move;
    move_list->count++;

    return true;
}