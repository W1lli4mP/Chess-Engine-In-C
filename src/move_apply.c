#include "move_apply.h"

bool make_move(GameState *game, Move move, UndoInfo *undo_out)
{
    if (!game || !game->board || !undo_out) return false;

    Board *board = game->board;
    if (!valid_move(board, move)) return false;

    // find pieces on source and destination squares
    Piece *piece = get_piece_at(board, move.from);
    if (!piece) return false;

    Piece *captured = get_piece_at(board, move.to);
    if (!captured) return false;

    // encapsulate relevant game state information into UndoInfo
    undo_out->captured_piece = captured;
    undo_out->captured_position = move.to;

    undo_out->previous_side_to_move = game->side_to_move;
    undo_out->previous_castling_rights = game->castling_rights;
    undo_out->previous_has_en_passant_target = game->has_en_passant_target;
    undo_out->previous_en_passant_target = game->en_passant_target;
    undo_out->previous_halfmove_clock = game->halfmove_clock;
    undo_out->previous_fullmove_number = game->fullmove_number;
    undo_out->previous_moved_piece_type = piece->type;

    // update board state
    set_piece_at(board, move.to, piece);
    set_piece_at(board, move.from, NULL);

    // update game state
    game->side_to_move = (game->side_to_move == COLOUR_WHITE) ? COLOUR_BLACK : COLOUR_WHITE;

    if (game->side_to_move == COLOUR_WHITE) game->fullmove_number++;

    game->has_en_passant_target = false;
    game->en_passant_target = (Position) { .row = -1, .col = -1 };

    if (piece->type == TYPE_PAWN || captured)
        game->halfmove_clock = 0;
    else
        game->halfmove_clock++;
    
    return true;
}


bool unmake_move(GameState *game, Move move, const UndoInfo *undo)
{
    if (!game || !game->board || !undo) return false;

    Board *board = game->board;

    Piece *piece = get_piece_at(board, move.to);
    if (!piece) return false;

    // undo piece state
    piece->type = undo->previous_moved_piece_type;
    piece->sprite = find_sprite(piece->type, piece->colour);

    // undo board state
    set_piece_at(board, move.from, piece);
    set_piece_at(board, move.to, undo->captured_piece);

    // undo game state
    game->side_to_move = undo->previous_side_to_move;
    game->castling_rights = undo->previous_castling_rights;
    game->has_en_passant_target = undo->previous_has_en_passant_target;
    game->en_passant_target = undo->previous_en_passant_target;
    game->halfmove_clock = undo->previous_halfmove_clock;
    game->fullmove_number = undo->previous_fullmove_number;

    return true;
}