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

// helper for checkmate/stalemate
static bool side_has_legal_move(const GameState *game, Colour colour);

// helper for determining whether a square is attacked - much faster approach than generating moves for each piece
bool is_square_attacked(const GameState *game, Position square, Colour friendly_colour)
{
    if (!game || !game->board) return false;

    return (
        attacked_by_knight(game->board, square, friendly_colour) ||
        attacked_by_king(game->board, square, friendly_colour) ||
        attacked_by_pawn(game->board, square, friendly_colour) ||
        attacked_by_orthogonal_piece(game->board, square, friendly_colour) ||
        attacked_by_diagonal_piece(game->board, square, friendly_colour)
    );
}

bool is_in_check(const GameState *game, Colour colour)
{
    if (!game || !game->board) return false;

    // retrieve king position and piece
    Position king_position;
    if (!find_king_position(game->board, colour, &king_position)) return false;

    return is_square_attacked(game, king_position, colour);
}

bool is_checkmate(const GameState *game, Colour colour)
{
    return is_in_check(game, colour) && !side_has_legal_move(game, colour);
}

bool is_stalemate(const GameState *game, Colour colour)
{
    return !is_in_check(game, colour) && !side_has_legal_move(game, colour);
}

// iterate through all pieces belonging to a side to verify if they at least one piece has a legal move
static bool side_has_legal_move(const GameState *game, Colour colour)
{
    if (!game || !game->board) return false;

    GameState temp_game = *game;
    temp_game.side_to_move = colour;

    const Board *board = temp_game.board;

    for (int row = 0; row < board->height; row++)
    {
        for (int col = 0; col < board->width; col++)
        {
            Position from = { .row = row, .col = col };
            Piece *piece = get_piece_at(board, from);

            if (!piece || piece->colour != colour) continue;

            MoveList moves = {0};

            if (!generate_legal_moves(&temp_game, from, &moves)) return false;

            if (moves.count > 0) return true;
        }
    }

    return false;
}