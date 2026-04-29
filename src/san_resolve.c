#include "san_resolve.h"
#include "rules.h"

/*
TODO:
handle castling
handle san suffix (checks/mates)
handle en passant
*/
static bool is_pawn_promotion_rank(const Piece *piece, Position to);
static bool verify_square(const Board *board, San san, Position from, Colour side_to_move, Piece **piece_out);
static ResolveStatus validate_moves(const GameState *game, San san, Piece *current_piece, PositionList move_list, Move *move_out);
static ResolveStatus validate_move(const Board *board, San san, Piece *current_piece, Position to, Move *move_out);

// helper for implicit pawn promotions
static bool is_pawn_promotion_rank(const Piece *piece, Position to)
{
    if (!piece || piece->type != TYPE_PAWN) return false;
    if (piece->colour == COLOUR_WHITE && to.row == 7) return true;
    if (piece->colour == COLOUR_BLACK && to.row == 0) return true;
    return false;
}


// verifies ONE square
static bool verify_square(const Board *board, San san, Position from, Colour side_to_move, Piece **piece_out)
{
    if (!board || !piece_out) return false;

    // disambiguation verification
    if (san.from_col != -1 && san.from_col != from.col) return false;
    if (san.from_row != -1 && san.from_row != from.row) return false;

    // piece and piece type verification
    Piece *current_piece = get_piece_at(board, from);

    if (!current_piece) return false;
    if (current_piece->type != san.piece) return false;
    if (current_piece->colour != side_to_move) return false;

    // extract piece
    *piece_out = current_piece;
    return true;
}

// populates a Move struct while verifying
// particularly: to
static ResolveStatus validate_moves(const GameState *game, San san, Piece *current_piece, PositionList move_list, Move *move_out)
{
    if (!game || !game->board || !current_piece || !move_out) return RESOLVE_ILLEGAL;

    const Board *board = game->board;

    bool found = false;

    // validate if any moves match the SAN (to)
    for (int i = 0; i < move_list.count; i++)
    {
        Position to = move_list.moves[i];
        if (to.col != san.to.col || to.row != san.to.row) continue;

        //* validate if the move match the SAN's other attributes
        ResolveStatus current_status = validate_move(board, san, current_piece, to, move_out);
        if (current_status != RESOLVE_OK) continue;

        // cannot have repeats
        if (found) return RESOLVE_AMBIGUOUS;

        move_out->to = to;
        found = true;
    }
    return found ? RESOLVE_OK : RESOLVE_ILLEGAL;
}

// helper that verifies a singular valid move based on SAN intent
// also populates a Move struct while verifying
// particularly: is_promotion, promotion
static ResolveStatus validate_move(const Board *board, San san, Piece *current_piece, Position to, Move *move_out)
{
    if (!board || !current_piece || !move_out) return RESOLVE_ILLEGAL;

    // captures (target can be an enemy or ally)
    Piece *target = get_piece_at(board, to);
    if (san.is_capture)
    {
        if (!target) return RESOLVE_ILLEGAL;
        if (target->colour == current_piece->colour) return RESOLVE_ILLEGAL;
    }
    else if (target) return RESOLVE_ILLEGAL;

    // explicit and implicit promotions
    if (san.is_promotion || is_pawn_promotion_rank(current_piece, to))
    {
        if (!is_pawn_promotion_rank(current_piece, to)) return RESOLVE_ILLEGAL;

        // default to queen if no promotion piece is explicitly given
        move_out->is_promotion = true;
        move_out->promotion = (san.promotion == TYPE_NONE) ? TYPE_QUEEN : san.promotion;
    }

    return RESOLVE_OK;
}

ResolveStatus resolve_san(const GameState *game, San san, Move *move_out)
{
    /*
        purpose:
        verify if the input SAN is a illegal, ambigious or ok
        populates a Move struct iff ok
    */

    if (!game || !game->board || !move_out) return RESOLVE_ILLEGAL;

    const Board *board = game->board;
    Colour side_to_move = game->side_to_move;

    // updates move_out safely
    Move found_move = {0};

    bool found = false;

    for (int row = 0; row < board->height; row++)
    {
        for (int col = 0; col < board->width; col++)
        {
            //? how it works: finds a move that satisfies the SAN, if there is more than one move, resolve as ambiguous, else populate move_out
            // verify square and extract piece if valid
            Position from = { .col = col, .row = row };
            Piece *current_piece = NULL;
            if (!verify_square(board, san, from, side_to_move, &current_piece)) continue;

            // generate moves
            PositionList moves = {0};
            if (!generate_legal_moves(game, from, &moves)) continue;

            //? i dont know what im doing anymore
            Move temp = {0};
            // update rest of temp
            temp.piece = san.piece;
            temp.from = from;
            // temp.to is decided in validate_moves()
            temp.is_capture = san.is_capture;
            temp.is_castle_kingside = san.is_castle_kingside;
            temp.is_castle_queenside = san.is_castle_queenside;
            // temp.is_promotion and temp.promotion is decided in validate_move()

            // validate the moves list of the current piece
            ResolveStatus piece_status = validate_moves(game, san, current_piece, moves, &temp);

            if (piece_status == RESOLVE_ILLEGAL) continue;

            if (piece_status == RESOLVE_AMBIGUOUS) return RESOLVE_AMBIGUOUS;

            if (found) return RESOLVE_AMBIGUOUS;

            found_move = temp;
            found = true;
        }
    }

    // finally, link the temp Move struct to the output Move struct
    if (found)
    {
        *move_out = found_move;
        return RESOLVE_OK;
    }

    return RESOLVE_ILLEGAL;
}