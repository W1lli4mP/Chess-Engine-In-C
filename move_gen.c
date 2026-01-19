#include "move_gen.h"

static bool generate_king_moves(const Board *board, Position piece_location, PositionList *position_list_out);

static bool in_bounds(const Board *board, int row, int col)
{
    return col >= 0 && col < board->width && row >= 0 && row < board->height;
}

static bool position_list_append(PositionList *position_list, Position move)
{
    if (position_list->count >= MAX_MOVES) return false;
    position_list->moves[position_list->count++] = move;
    return true;
}

bool generate_pseudo_legal_moves(const Board *board, Position piece_location, PositionList *position_list_out)
{
    if (!position_list_out) return false;

    Piece *selected_piece = get_piece_at(board, piece_location);
    if (!selected_piece) return false;

    // generate moves based on the piece type
    if (selected_piece->type == TYPE_KING)
    {
        return generate_king_moves(board, piece_location, position_list_out);
    }
    else return false;
}

static bool generate_king_moves(const Board *board, Position piece_location, PositionList *position_list_out)
{
    static const int d[8][2] = { {0, 1}, {1, 1}, {1, 0}, {1, -1},
                                {0, -1}, {-1, -1}, {-1, 0}, {-1, 1} };

    for (int i = 0; i < 8; i++)
    {
        int col = piece_location.col + d[i][0];
        int row = piece_location.row + d[i][1];

        // skip if out of bounds
        if (!in_bounds(board, row, col)) continue;

        // select piece at piece location
        Piece *selected_piece = get_piece_at(board, piece_location);

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

// TODO: update to handle checks and checkmates
bool generate_legal_moves(const Board *board, Position piece_location, PositionList *position_list_out)
{
    return generate_pseudo_legal_moves(board, piece_location, position_list_out);
}

// where i left off: generate king moves