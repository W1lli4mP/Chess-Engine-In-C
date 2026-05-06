#include "san_resolve.h"
#include "move_gen.h"

/*
TODO:
handle castling
handle san suffix (checks/mates)
handle en passant
*/
static bool is_pawn_promotion_rank(const Piece *piece, Position to);

static bool verify_square(
    const Board *board,
    San san,
    Position from,
    Colour side_to_move,
    Piece **piece_out
);

static ResolveStatus validate_moves(
    San san,
    Piece *current_piece,
    MoveList move_list,
    Move *move_out
);

static ResolveStatus validate_move(
    San san,
    Piece *current_piece,
    Move candidate,
    Move *move_out
);

// helper for implicit pawn promotions
static bool is_pawn_promotion_rank(const Piece *piece, Position to)
{
    if (!piece || piece->type != TYPE_PAWN) return false;
    if (piece->colour == COLOUR_WHITE && to.row == 7) return true;
    if (piece->colour == COLOUR_BLACK && to.row == 0) return true;
    return false;
}


// verifies ONE square
static bool verify_square(
    const Board *board,
    San san,
    Position from,
    Colour side_to_move,
    Piece **piece_out
)
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

// finds exactly ONE generated legal move match the SAN
static ResolveStatus validate_moves(
    San san,
    Piece *current_piece,
    MoveList move_list,
    Move *move_out
)
{
    if (!current_piece || !move_out) return RESOLVE_ILLEGAL;

    bool found = false;

    // validate if any moves match the SAN (to)
    for (int i = 0; i < move_list.count; i++)
    {
        Move candidate = move_list.moves[i];

        if (candidate.to.col != san.to.col || candidate.to.row != san.to.row) continue;

        //* validate if the move match the SAN's other attributes
        ResolveStatus current_status = validate_move(san, current_piece, candidate, move_out);

        if (current_status != RESOLVE_OK) continue;

        // cannot have repeats
        if (found) return RESOLVE_AMBIGUOUS;

        found = true;
    }

    return found ? RESOLVE_OK : RESOLVE_ILLEGAL;
}

// verifies one generated legal move against the SAN intent
// and copies it into move_out if valid
static ResolveStatus validate_move(
    San san,
    Piece *current_piece,
    Move candidate,
    Move *move_out
)
{
    if (!current_piece || !move_out) return RESOLVE_ILLEGAL;

    Position to = candidate.to;

    // captures (target can be an enemy or ally)
    if (san.is_capture != candidate.is_capture) return RESOLVE_ILLEGAL;

    // explicit and implicit promotions
    bool reaches_promotion_rank = is_pawn_promotion_rank(current_piece, to);

    if (san.is_promotion)
    {
        if (!reaches_promotion_rank) return RESOLVE_ILLEGAL;

        candidate.is_promotion = true;
        candidate.promotion = san.promotion;
    }
    else if (reaches_promotion_rank)
    {
        //! defaults to queen if no promotion piece is explicitly given
        candidate.is_promotion = true;
        candidate.promotion = TYPE_QUEEN;
    }

    *move_out = candidate;
    return RESOLVE_OK;
}

ResolveStatus resolve_san(GameState *game, San san, Move *move_out)
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
            //? finds source pieces whose legal moves match the SAN
            //? more than one match means the SAN is ambiguous
            // verify square and extract piece if valid
            Position from = { .col = col, .row = row };

            Piece *current_piece = NULL;
            if (!verify_square(board, san, from, side_to_move, &current_piece)) continue;

            // generate moves
            MoveList moves = {0};

            if (!generate_legal_moves(game, from, &moves)) continue;

            Move temp = {0};

            // validate the moves list of the current piece
            ResolveStatus piece_status = validate_moves(san, current_piece, moves, &temp);

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