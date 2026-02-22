#include "rules.h"

static bool find_king_position(const Board *board, Colour king_colour, Position *king_position_out);

static bool attacked_by_knight(const Board *board, Position square, Colour friendly_colour);
static bool attacked_by_king(const Board *board, Position square, Colour friendly_colour);
static bool attacked_by_pawn(const Board *board, Position square, Colour friendly_colour);

static bool attacked_by_sliding_piece(const Board *board, Position square, Colour friendly_colour, const int d[4][2], PieceType target_type);
static bool attacked_by_orthogonal_piece(const Board *board, Position square, Colour friendly_colour);
static bool attacked_by_diagonal_piece(const Board *board, Position square, Colour friendly_colour);

// assumes one king exists per colour
static bool find_king_position(const Board *board, Colour king_colour, Position *king_position_out)
{
    // iterate through all squares
    for (int row = 0; row < board->height; row++)
    {
        for (int col = 0; col < board->width; col++)
        {
            // find king piece that matches the desired colour
            Position current_position = { .col = col, .row = row };
            Piece *selected_piece = get_piece_at(board, current_position);

            // skip pieces that do not exist/fit the description
            if (!selected_piece || selected_piece->type != TYPE_KING || selected_piece->colour != king_colour) continue;
    
            king_position_out->col = col;
            king_position_out->row = row;
            return true;
        }
    }
    return false;
}

// mini helpers for finding attackers
static bool attacked_by_knight(const Board *board, Position square, Colour friendly_colour)
{
    static const int d[8][2] = { {1, 2}, {2, 1}, {2, -1}, {1, -2},
                              {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2} };

    for (int i = 0; i < 8; i++)
    {
        // apply directions
        int col = square.col + d[i][0];
        int row = square.row + d[i][1];

        if (!in_bounds(board, row, col)) continue;

        Position to = { .col = col, .row = row };

        Piece *target = get_piece_at(board, to);

        // if enemy piece found, king is attacked
        if (target && target->colour != friendly_colour && target->type == TYPE_KNIGHT) return true;
    }
    return false;
}

static bool attacked_by_king(const Board *board, Position square, Colour friendly_colour)
{
    static const int d[8][2] = { {0, 1}, {1, 1}, {1, 0}, {1, -1},
                                {0, -1}, {-1, -1}, {-1, 0}, {-1, 1} };

    for (int i = 0; i < 8; i++)
    {
        int col = square.col + d[i][0];
        int row = square.row + d[i][1];

        // skip if out of bounds
        if (!in_bounds(board, row, col)) continue;

        // find piece at target destination
        Position to = { .row = row, .col = col };

        Piece *target = get_piece_at(board, to);

        if (target && target->colour != friendly_colour && target->type == TYPE_KING) return true;
    }
    return false;
}

static bool attacked_by_pawn(const Board *board, Position square, Colour friendly_colour)
{
    // establish directions
    int d = (friendly_colour == COLOUR_WHITE) ? -1 : 1;

    int l_col = square.col - 1, r_col = square.col + 1;
    int row = square.row + d;

    Position left = { .col = l_col, .row = row };
    Position right = { .col = r_col, .row = row };

    // verify diagonal pieces
    Piece *left_piece = get_piece_at(board, left);
    Piece *right_piece = get_piece_at(board, right);

    return ((left_piece && left_piece->type == TYPE_PAWN && left_piece->colour != friendly_colour) ||
            (right_piece && right_piece->type == TYPE_PAWN && right_piece->colour != friendly_colour));
}

static bool attacked_by_sliding_piece(const Board *board, Position square, Colour friendly_colour, const int d[4][2], PieceType target_type)
{
    // iterate and "slide" through each direction possible
    for (int i = 0; i < 4; i++)
    {
        // col and row incrementers
        int dy = 1, dx = 1;

        // iterate until piece cannot "slide" any longer (due to colliding with another piece or going out of bounds)
        for (;;)
        {
            // apply direction + incrementer
            int col = square.col + d[i][0] * dy;
            int row = square.row + d[i][1] * dx;

            Position to = { .col = col, .row = row };

            // check if move is in bounds
            if (!in_bounds(board, row, col)) break;

            // select piece on destination
            Piece *target = get_piece_at(board, to);

            // check for enemy pieces and whether are of the correct piece type
            if (target && target->colour != friendly_colour)
            {
                if (target->type == target_type || target->type == TYPE_QUEEN) return true;
                else break;
            }

            // check for friendly pieces
            if (target && target->colour == friendly_colour) break;

            // else continue "sliding"
            dy++;
            dx++;
        }
    }
    return false;
}

static bool attacked_by_orthogonal_piece(const Board *board, Position square, Colour friendly_colour)
{
    // rooks + queens
    static const int d[4][2] = { {1, 0}, {0, -1}, {-1, 0}, {0, 1} };

    return attacked_by_sliding_piece(board, square, friendly_colour, d, TYPE_ROOK);
}

static bool attacked_by_diagonal_piece(const Board *board, Position square, Colour friendly_colour)
{
    // bishops + queens
    static const int d[4][2] = { {1, 1}, {1, -1}, {-1, -1}, {-1, 1} };

    return attacked_by_sliding_piece(board, square, friendly_colour, d, TYPE_BISHOP);
}

// helper for determining whether a square is attacked - much faster approach than generating moves for each piece
bool is_square_attacked(const Board *board, Position square, Colour friendly_colour)
{
    return (
        attacked_by_knight(board, square, friendly_colour) ||
        attacked_by_king(board, square, friendly_colour) ||
        attacked_by_pawn(board, square, friendly_colour) ||
        attacked_by_orthogonal_piece(board, square, friendly_colour) ||
        attacked_by_diagonal_piece(board, square, friendly_colour)
    );
}

bool is_in_check(const Board *board, Colour colour)
{
    // retrieve king position and piece
    Position king_position;
    if (!find_king_position(board, colour, &king_position)) return false;

    return is_square_attacked(board, king_position, colour);
}

bool is_checkmate(const Board *board, Colour colour)
{
    // king must be in check in order to be in checkmate
    if (!is_in_check(board, colour)) return false;

    // find king position
    Position king_position;
    if (!find_king_position(board, colour, &king_position)) return false;

    // king must have no moves as well
    PositionList move_list = {0}; // initialise with count = 0 and moves = 0
    if (!generate_legal_moves(board, king_position, &move_list)) return false;

    return (move_list.count == 0);
}

bool is_stalemate(const Board *board, Colour colour)
{
    // king must not be in check in order to be in stalemate
    if (is_in_check(board, colour)) return false;

    // find king position
    Position king_position;
    if (!find_king_position(board, colour, &king_position)) return false;

    // king must have no moves as well
    PositionList move_list = {0}; // initialise with count = 0 and moves = 0
    if (!generate_legal_moves(board, king_position, &move_list)) return false;

    return (move_list.count == 0);
}

static void position_to_move(const Board *board, Position from, Position to, Move *move_out)
{
    Piece *selected_piece = get_piece_at(board, from);

    move_out->piece = selected_piece->type;
    move_out->from = from;
    move_out->to = to;
    
    // TODO: update other attributes in future
}

// TODO: update to handle checks and checkmates
bool generate_legal_moves(const Board *board, Position piece_location, PositionList *position_list_out)
{
    // create a copy of the board
    Board temp_copy = *board;

    // find piece for information
    Piece *selected_piece = get_piece_at(&temp_copy, piece_location);

    // calculate pseudo legal moves
    PositionList pseudo_moves = {0};

    if (!generate_pseudo_legal_moves(&temp_copy, piece_location, &pseudo_moves)) return false;

    // simulate every move
    for (int i = 0; i < pseudo_moves.count; i++)
    {
        Move move = {0};

        // convert Position into Move struct
        position_to_move(&temp_copy, piece_location, pseudo_moves.moves[i], &move);

        // play the move
        if (!simulate_move(&temp_copy, move)) return false;

        // check if move does not leave king in check in order to append, discard move otherwise
        if (!is_in_check(&temp_copy, selected_piece->colour))
        {
            if (!position_list_append(position_list_out, pseudo_moves.moves[i])) return false;
        }

        // reset copy back to default
        temp_copy = *board;
    }
    return true;
}