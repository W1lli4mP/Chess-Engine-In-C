#include "move_apply.h"

static void update_castling_rights_after_move(
    GameState *game,
    Move move,
    Piece *piece,
    Piece *captured
);

static void update_en_passant_target_after_move(
    GameState *game,
    Move move,
    Piece *piece
);

static void update_castling_rights_after_move(
    GameState *game,
    Move move,
    Piece *piece,
    Piece *captured
)
{
    if (!game || !piece) return;

    // if a king moves, loses both rights for it's colour
    if (piece->type == TYPE_KING)
    {
        if (piece->colour == COLOUR_WHITE)
        {
            game->castling_rights.white_can_castle_kingside = false;
            game->castling_rights.white_can_castle_queenside = false;
        }
        else if (piece->colour == COLOUR_BLACK)
        {
            game->castling_rights.black_can_castle_kingside = false;
            game->castling_rights.black_can_castle_queenside = false;
        }
    }

    // if a rook moves, loses one right for it's colour
    if (piece->type == TYPE_ROOK)
    {
        if (move.from.row == 0 && move.from.col == 0)
            game->castling_rights.white_can_castle_queenside = false;
        else if (move.from.row == 0 && move.from.col == 7)
            game->castling_rights.white_can_castle_kingside = false;
        else if (move.from.row == 7 && move.from.col == 0)
            game->castling_rights.black_can_castle_queenside = false;
        else if (move.from.row == 7 && move.from.col == 7)
            game->castling_rights.black_can_castle_kingside = false;
    }

    // if a rook is captured, loses one right for it's colour
    if (captured && captured->type == TYPE_ROOK)
    {
        if (move.to.row == 0 && move.to.col == 0)
            game->castling_rights.white_can_castle_queenside = false;
        else if (move.to.row == 0 && move.to.col == 7)
            game->castling_rights.white_can_castle_kingside = false;
        else if (move.to.row == 7 && move.to.col == 0)
            game->castling_rights.black_can_castle_queenside = false;
        else if (move.to.row == 7 && move.to.col == 7)
            game->castling_rights.black_can_castle_kingside = false;
    }
}

static void update_en_passant_target_after_move(
    GameState *game,
    Move move,
    Piece *piece
)
{
    if (!game || !game->board || !piece) return;

    // update en passant for pawn moves only
    if (piece->type != TYPE_PAWN) return;

    // check for double-forward
    bool double_forward = 
        (piece->colour == COLOUR_WHITE && move.from.row == 1 && move.to.row == 3) ||
        (piece->colour == COLOUR_BLACK && move.from.row == 6 && move.to.row == 4);
    
    if (!double_forward) return;

    // set en passant target
    // en passant target's row will always be the midpoint of the previous and current location
    int target_row = (move.from.row + move.to.row) / 2;

    game->has_en_passant_target = true;
    game->en_passant_target = (Position) { .row = target_row, .col = move.from.col };
}

bool make_move(GameState *game, Move move, UndoInfo *undo_out)
{
    if (!game || !game->board || !undo_out) return false;

    Board *board = game->board;
    if (!is_move_on_board(board, move)) return false;

    // find pieces on source and destination squares
    Piece *piece = get_piece_at(board, move.from);
    if (!piece) return false;

    Piece *captured = get_piece_at(board, move.to);
    // captured can be NULL so don't add a NULL check

    // promotion validation
    if (move.is_promotion && piece->type != TYPE_PAWN) return false;

    if (
        move.is_promotion &&
        move.promotion != TYPE_QUEEN &&
        move.promotion != TYPE_ROOK &&
        move.promotion != TYPE_BISHOP &&
        move.promotion != TYPE_KNIGHT
    ) return false;

    if (move.is_promotion)
    {
        bool valid_rank = 
            (piece->colour == COLOUR_WHITE && move.to.row == 7) ||
            (piece->colour == COLOUR_BLACK && move.to.row == 0);

        if (!valid_rank) return false;
    }

    // handle castling
    if (move.is_castle_kingside || move.is_castle_queenside)
    {
        // initial castling validation
        if (piece->type != TYPE_KING) return false;
        if (captured || move.is_capture) return false; // cannot capture AND castle
        if (move.is_castle_kingside && move.is_castle_queenside) return false;
        if (move.from.row != move.to.row) return false;

        if (move.is_castle_kingside && move.to.col != move.from.col + 2) return false;
        if (move.is_castle_queenside && move.to.col != move.from.col - 2) return false;

        // actual logic
        int row = move.from.row;

        Position rook_from = move.is_castle_kingside
            ? (Position) { .row = row, .col = 7 }
            : (Position) { .row = row, .col = 0 };
        
        Position rook_to = move.is_castle_kingside
            ? (Position) { .row = row, .col = 5 }
            : (Position) { .row = row, .col = 3 };

        Piece *rook = get_piece_at(board, rook_from);

        if (!rook || rook->type != TYPE_ROOK || rook->colour != piece->colour) return false;

        undo_out->castling_rook = rook;
        undo_out->castling_rook_from = rook_from;
        undo_out->castling_rook_to = rook_to;
    }
    else
    {
        undo_out->castling_rook = NULL;
        undo_out->castling_rook_from = (Position) { .row = -1, .col = -1 };
        undo_out->castling_rook_to = (Position) { .row = -1, .col = -1 };
    }

    //* en passant validation
    // reject invalid/impossible en passant states
    if (move.is_en_passant && !game->has_en_passant_target) return false;

    if (
        move.is_en_passant &&
        (move.to.row != game->en_passant_target.row ||
        move.to.col != game->en_passant_target.col)
    ) return false;

    if (move.is_en_passant)
    {
        int d = (piece->colour == COLOUR_WHITE) ? 1 : -1;

        if (piece->type != TYPE_PAWN) return false;
        if (!move.is_capture) return false;
        if (move.to.row != move.from.row + d) return false;
        if (move.to.col != move.from.col - 1 && move.to.col != move.from.col + 1) return false;
    }

    // encapsulate relevant game state information into UndoInfo
    //* if en passant, captured pawn is on a different square
    if (move.is_en_passant)
    {
        Position captured_location = { .row = move.from.row, .col = move.to.col };

        // reject if the destination is occupied
        if (get_piece_at(board, move.to)) return false;

        captured = get_piece_at(board, captured_location);

        if (!captured || captured->type != TYPE_PAWN || captured->colour == piece->colour) return false;

        undo_out->captured_piece = captured;
        undo_out->captured_position = captured_location;
    }
    else
    {
        captured = get_piece_at(board, move.to);
        undo_out->captured_piece = captured;
        undo_out->captured_position = move.to;
    }

    undo_out->previous_side_to_move = game->side_to_move;
    undo_out->previous_castling_rights = game->castling_rights;
    undo_out->previous_has_en_passant_target = game->has_en_passant_target;
    undo_out->previous_en_passant_target = game->en_passant_target;
    undo_out->previous_halfmove_clock = game->halfmove_clock;
    undo_out->previous_fullmove_number = game->fullmove_number;
    undo_out->previous_moved_piece_type = piece->type;

    // update board state
    if (!set_piece_at(board, move.to, piece)) return false;
    if (!set_piece_at(board, move.from, NULL)) return false;

    //* promote pawns if move is a promotion
    if (move.is_promotion)
    {
        piece->type = move.promotion;
        piece->sprite = find_sprite(piece->type, piece->colour);
    }

    //* move the rook if move is a castle
    if (move.is_castle_kingside || move.is_castle_queenside)
    {
        if (!set_piece_at(board, undo_out->castling_rook_to, undo_out->castling_rook)) return false;
        if (!set_piece_at(board, undo_out->castling_rook_from, NULL)) return false;
    }

    //* remove captured pawn if move is an en passant
    if (move.is_en_passant)
    {
        if (!set_piece_at(board, undo_out->captured_position, NULL)) return false;
    }

    // update castling rights
    update_castling_rights_after_move(game, move, piece, captured);

    //* update en passant
    // clear en passant before updating
    game->has_en_passant_target = false;
    game->en_passant_target = (Position) { .row = -1, .col = -1 };

    update_en_passant_target_after_move(game, move, piece);

    // update game state
    game->side_to_move = (game->side_to_move == COLOUR_WHITE) ? COLOUR_BLACK : COLOUR_WHITE;

    if (game->side_to_move == COLOUR_WHITE) game->fullmove_number++;

    // use the saved previous piece type; pawn promotions may affect the current state
    if (undo_out->previous_moved_piece_type == TYPE_PAWN || captured)
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

    if (undo->castling_rook)
    {
        // revert castled rook
        if (!set_piece_at(board, undo->castling_rook_from, undo->castling_rook)) return false;
        if (!set_piece_at(board, undo->castling_rook_to, NULL)) return false;
    }

    // undo piece state
    piece->type = undo->previous_moved_piece_type;
    piece->sprite = find_sprite(piece->type, piece->colour);

    // undo board state
    if (!set_piece_at(board, move.from, piece)) return false;

    if (move.is_en_passant)
    {
        if (!set_piece_at(board, move.to, NULL)) return false;
        if (!set_piece_at(board, undo->captured_position, undo->captured_piece)) return false;
    }
    else
    {
        if (!set_piece_at(board, move.to, undo->captured_piece)) return false;
    }

    // undo game state
    game->side_to_move = undo->previous_side_to_move;
    game->castling_rights = undo->previous_castling_rights;
    game->has_en_passant_target = undo->previous_has_en_passant_target;
    game->en_passant_target = undo->previous_en_passant_target;
    game->halfmove_clock = undo->previous_halfmove_clock;
    game->fullmove_number = undo->previous_fullmove_number;

    return true;
}