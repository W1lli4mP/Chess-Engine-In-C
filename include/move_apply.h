#ifndef MOVE_APPLY_H
#define MOVE_APPLY_H

#include <stdbool.h>
#include "game_state.h"
#include "move.h"
#include "position.h"

typedef struct
{
    Piece *captured_piece;
    Position captured_position;

    Colour previous_side_to_move;

    CastlingRights previous_castling_rights;

    Piece *castling_rook;
    Position castling_rook_from;
    Position castling_rook_to;

    bool previous_has_en_passant_target;
    Position previous_en_passant_target;

    int previous_halfmove_clock;
    int previous_fullmove_number;

    PieceType previous_moved_piece_type;
} UndoInfo;

bool make_move(GameState *game, Move move, UndoInfo *undo_out);
bool unmake_move(GameState *game, Move move, const UndoInfo *undo);

#endif