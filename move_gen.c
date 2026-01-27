#include "move_gen.h"

static bool generate_king_moves(const Board *board, Position piece_location, PositionList *position_list_out);
static bool generate_pawn_moves(const Board *board, Position piece_location, PositionList *position_list_out);
static bool generate_knight_moves(const Board *board, Position piece_location, PositionList *position_list_out);

static bool generate_sliding_moves(const Board *board, Position piece_location, PositionList *position_list_out, const int d[][2], const int num_directions);
static bool generate_bishop_moves(const Board *board, Position piece_location, PositionList *position_list_out);
static bool generate_rook_moves(const Board *board, Position piece_location, PositionList *position_list_out);
static bool generate_queen_moves(const Board *board, Position piece_location, PositionList *position_list_out);

bool generate_pseudo_legal_moves(const Board *board, Position piece_location, PositionList *position_list_out)
{
    if (!position_list_out) return false;

    Piece *selected_piece = get_piece_at(board, piece_location);
    if (!selected_piece) return false;

    // generate moves based on the piece type
    if (selected_piece->type == TYPE_PAWN)
    {
        return generate_pawn_moves(board, piece_location, position_list_out);
    }
    if (selected_piece->type == TYPE_KING)
    {
        return generate_king_moves(board, piece_location, position_list_out);
    }
    if (selected_piece->type == TYPE_KNIGHT)
    {
        return generate_knight_moves(board, piece_location, position_list_out);
    }
    if (selected_piece->type == TYPE_BISHOP)
    {
        return generate_bishop_moves(board, piece_location, position_list_out);
    }
    if (selected_piece->type == TYPE_ROOK)
    {
        return generate_rook_moves(board, piece_location, position_list_out);
    }
    if (selected_piece->type == TYPE_QUEEN)
    {
        return generate_queen_moves(board, piece_location, position_list_out);
    }
    else return false;
}

// TODO: add en passant
static bool generate_pawn_moves(const Board *board, Position piece_location, PositionList *position_list_out)
{
    Piece *selected_piece = get_piece_at(board, piece_location);

    // determine pawn direction based on colour
    int d = (selected_piece->colour == COLOUR_WHITE) ? 1 : -1;

    int col = piece_location.col;
    int row = piece_location.row;

    // forward
    Position f1 = { .row = row + d, .col = col };

    Piece *target = get_piece_at(board, f1);

    // can only move forward to empty squares
    if (!target)
    {
        if (!position_list_append(position_list_out, f1)) return false;
    
        // double forward (iff original position was at a starting position)
        if ((selected_piece->colour == COLOUR_WHITE && piece_location.row == 1) || (selected_piece->colour == COLOUR_BLACK && piece_location.row == board->height - 2))
        {
            Position f2 = { .row = row + 2 * d, .col = col };

            Piece *target2 = get_piece_at(board, f2);
            if (!target2)
            {
                if (!position_list_append(position_list_out, f2)) return false;
            }

        }
    }
    return true;
}

static bool generate_king_moves(const Board *board, Position piece_location, PositionList *position_list_out)
{
    static const int d[8][2] = { {0, 1}, {1, 1}, {1, 0}, {1, -1},
                                {0, -1}, {-1, -1}, {-1, 0}, {-1, 1} };

    // select piece at piece location
    Piece *selected_piece = get_piece_at(board, piece_location);

    for (int i = 0; i < 8; i++)
    {
        int col = piece_location.col + d[i][0];
        int row = piece_location.row + d[i][1];

        // skip if out of bounds
        if (!in_bounds(board, row, col)) continue;

        // find piece at target destination
        Position to = { .row = row, .col = col };

        Piece *target = get_piece_at(board, to);

        // selected piece cannot capture friendly pieces - dont append
        if (target && selected_piece->colour == target->colour) continue;

        // attempt to append move, returning false if failing
        if (!position_list_append(position_list_out, to)) return false;
    }
    // once all possible moves have been processed successfully, return true
    return true;
}

static bool generate_knight_moves(const Board *board, Position piece_location, PositionList *position_list_out)
{
    static const int d[8][2] = { {1, 2}, {2, 1}, {2, -1}, {1, -2},
                              {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2} };

    // locate knight
    Piece *selected_piece = get_piece_at(board, piece_location);

    for (int i = 0; i < 8; i++)
    {
        // apply directions
        int col = piece_location.col + d[i][0];
        int row = piece_location.row + d[i][1];

        if (!in_bounds(board, row, col)) continue;

        Position to = { .col = col, .row = row };

        Piece *target = get_piece_at(board, to);

        // friendly piece check
        if (target && selected_piece->colour == target->colour) continue;

        if(!position_list_append(position_list_out, to)) return false;
    }
    return true;
}

// helper for sliding pieces: bishops, rooks and queens
static bool generate_sliding_moves(const Board *board, Position piece_location, PositionList *position_list_out, const int d[][2], const int num_directions)
{
    Piece *selected_piece = get_piece_at(board, piece_location);

    // iterate and "slide" through each direction possible
    for (int i = 0; i < num_directions; i++)
    {
        // col and row incrementers
        int dy = 1, dx = 1;

        // iterate until piece cannot "slide" any longer (due to colliding with another piece or going out of bounds)
        for (;;)
        {
            // apply direction + incrementer
            int col = piece_location.col + d[i][0] * dy;
            int row = piece_location.row + d[i][1] * dx;

            Position to = { .col = col, .row = row };

            // check if move is in bounds
            if (!in_bounds(board, row, col)) break;

            // select piece on destination
            Piece *target = get_piece_at(board, to);

            // check for friendly pieces
            if (target && selected_piece->colour == target->colour) break;
            
            // append (only options are empty square or enemy piece)
            if (!position_list_append(position_list_out, to)) return false;

            // check for enemy pieces (to break early)
            if (target && selected_piece->colour != target->colour) break;

            // else continue "sliding"
            dy++;
            dx++;
        }
    }
    return true;
}

// sliding piece movement
static bool generate_bishop_moves(const Board *board, Position piece_location, PositionList *position_list_out)
{
    static const int num_directions = 4;
    static const int d[4][2] = { {1, 1}, {1, -1}, {-1, -1}, {-1, 1} };
    
    return generate_sliding_moves(board, piece_location, position_list_out, d, num_directions);
}

static bool generate_rook_moves(const Board *board, Position piece_location, PositionList *position_list_out)
{
    static const int num_directions = 4;
    static const int d[4][2] = { {1, 0}, {0, -1}, {-1, 0}, {0, 1} };

    return generate_sliding_moves(board, piece_location, position_list_out, d, num_directions);
}

static bool generate_queen_moves(const Board *board, Position piece_location, PositionList *position_list_out)
{
    // combination of bishop and rook directions
    static const int num_directions = 8;
    static const int d[8][2] = { {1, 1}, {1, -1}, {-1, -1}, {-1, 1},
                                 {1, 0}, {0, -1}, {-1, 0}, {0, 1} };

    return generate_sliding_moves(board, piece_location, position_list_out, d, num_directions);
}