#include "move_gen.h"
#include "rules.h"

static bool append_normal_move(
    const Board *board,
    Position from,
    Position to,
    MoveList *moves_out
);

static bool generate_pawn_moves(
    const GameState *game,
    Position piece_location,
    MoveList *moves_out
);

static bool generate_king_moves(
    const GameState *game,
    Position piece_location,
    MoveList *moves_out
);

static bool generate_knight_moves(
    const GameState *game,
    Position piece_location,
    MoveList *moves_out
);

static bool generate_sliding_moves(
    const GameState *game,
    Position piece_location,
    MoveList *moves_out,
    const int d[][2],
    int num_directions
);

static bool generate_bishop_moves(
    const GameState *game,
    Position piece_location,
    MoveList *moves_out
);

static bool generate_rook_moves(
    const GameState *game,
    Position piece_location,
    MoveList *moves_out
);

static bool generate_queen_moves(
    const GameState *game,
    Position piece_location,
    MoveList *moves_out
);

static bool is_empty_square(const Board *board, Position position);

//* MAIN HELPER
// constructs Move from two Position's and appends if possible
static bool append_normal_move(
    const Board *board,
    Position from,
    Position to,
    MoveList *moves_out
)
{
    // null check
    if (!board || !moves_out) return false;

    // retrieve piece
    Piece *piece = get_piece_at(board, from);
    if (!piece) return false;

    // create move
    Move move = create_move(piece->type, from, to);

    // add potential capture
    Piece *target = get_piece_at(board, to);
    if (target) move.is_capture = true;

    return move_list_append(moves_out, move);
}

//* MAIN FUNCTIONS
bool generate_pseudo_legal_moves(
    const GameState *game,
    Position piece_location,
    MoveList *moves_out
)
{
    if (!game || !game->board || !moves_out) return false;

    Piece *selected_piece = get_piece_at(game->board, piece_location);
    if (!selected_piece) return false;

    // generate moves based on the piece type
    switch (selected_piece->type)
    {
        case TYPE_PAWN:
            return generate_pawn_moves(game, piece_location, moves_out);

        case TYPE_KING:
            return generate_king_moves(game, piece_location, moves_out);

        case TYPE_KNIGHT:
            return generate_knight_moves(game, piece_location, moves_out);

        case TYPE_BISHOP:
            return generate_bishop_moves(game, piece_location, moves_out);

        case TYPE_ROOK:
            return generate_rook_moves(game, piece_location, moves_out);

        case TYPE_QUEEN:
            return generate_queen_moves(game, piece_location, moves_out);

        default:
            return false;
    }
}

bool generate_legal_moves(
    const GameState *game,
    Position piece_location,
    MoveList *moves_out
)
{
    if (!game || !game->board || !moves_out) return false;

    const Board *board = game->board;

    // find piece for information
    Piece *selected_piece = get_piece_at(board, piece_location);
    if (!selected_piece) return false;

    // calculate pseudo legal moves
    MoveList pseudo_moves = {0};

    if (!generate_pseudo_legal_moves(game, piece_location, &pseudo_moves)) return false;

    // simulate every move
    for (int i = 0; i < pseudo_moves.count; i++)
    {
        Move move = pseudo_moves.moves[i];

        Board temp_board = *board;

        // play the move
        if (!simulate_move(&temp_board, move)) return false;

        GameState temp_game = *game;
        temp_game.board = &temp_board;

        // don't append if move leaves king in check
        if (!is_in_check(&temp_game, selected_piece->colour))
        {
            if (!move_list_append(moves_out, move)) return false;
        }
    }

    return true;
}

bool generate_all_legal_moves(
    const GameState *game,
    MoveList *moves_out
)
{
    if (!game || !game->board || !moves_out) return false;

    const Board *board = game->board;

    // iterate through entire board
    for (int row = 0; row < board->height; row++)
    {
        for (int col = 0; col < board->width; col++)
        {
            Position from = { .row = row, .col = col };

            // retrieve pieces of the side's turn to move
            Piece *piece = get_piece_at(board, from);
            if (!piece) continue;

            if (piece->colour != game->side_to_move) continue;

            MoveList piece_moves = {0};

            if (!generate_legal_moves(game, from, &piece_moves)) return false;

            for (int i = 0; i < piece_moves.count; i++)
            {
                // append all generated legal moves to moves_out
                if (!move_list_append(moves_out, piece_moves.moves[i])) return false;
            }

        }
    }

    return true;
}

//* HELPERS
// TODO: add en passant
static bool generate_pawn_moves(
    const GameState *game,
    Position piece_location,
    MoveList *moves_out
)
{
    const Board *board = game->board;

    Piece *selected_piece = get_piece_at(board, piece_location);
    if (!selected_piece) return false;

    // determine pawn direction based on colour
    int d = (selected_piece->colour == COLOUR_WHITE) ? 1 : -1;

    int col = piece_location.col;
    int row = piece_location.row;

    // forward
    Position f1 = { .row = row + d, .col = col };

    // can only move forward to empty squares
    if (is_empty_square(board, f1))
    {
        if (!append_normal_move(board, piece_location, f1, moves_out)) return false;

        // double forward (iff original position was at a starting position)
        if ((selected_piece->colour == COLOUR_WHITE && piece_location.row == 1) || (selected_piece->colour == COLOUR_BLACK && piece_location.row == board->height - 2))
        {
            // repeat the same but for second forward move
            Position f2 = { .row = row + 2 * d, .col = col };

            if (is_empty_square(board, f2))
            {
                if (!append_normal_move(board, piece_location, f2, moves_out)) return false;
            }
        }
    }

    // check for diagonal captures
    Position captures[2] = {
        { .row = row + d, .col = col - 1 },
        { .row = row + d, .col = col + 1 }
    };

    // check both left and right diagonal captures
    for (int i = 0; i < 2; i++)
    {
        Position to = captures[i];

        Piece *target = get_piece_at(board, to);

        // append enemies if valid
        if (target && target->colour != selected_piece->colour)
        {
            if (!append_normal_move(board, piece_location, to, moves_out)) return false;
        }
    }

    return true;
}

static bool generate_king_moves(
    const GameState *game,
    Position piece_location,
    MoveList *moves_out
)
{
    const Board *board = game->board;

    static const int d[8][2] = {
        {0, 1}, {1, 1}, {1, 0}, {1, -1},
        {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}
    };

    // select piece at piece location
    Piece *selected_piece = get_piece_at(board, piece_location);
    if (!selected_piece) return false;

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
        if (!append_normal_move(board, piece_location, to, moves_out)) return false;
    }

    // once all possible moves have been processed successfully, return true
    return true;
}

static bool generate_knight_moves(
    const GameState *game,
    Position piece_location,
    MoveList *moves_out
)
{
    const Board *board = game->board;

    static const int d[8][2] = { {1, 2}, {2, 1}, {2, -1}, {1, -2},
                              {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2} };

    // locate knight
    Piece *selected_piece = get_piece_at(board, piece_location);
    if (!selected_piece) return false;

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

        if (!append_normal_move(board, piece_location, to, moves_out)) return false;
    }

    return true;
}

// helper for sliding pieces: bishops, rooks and queens
static bool generate_sliding_moves(
    const GameState *game,
    Position piece_location,
    MoveList *moves_out,
    const int d[][2],
    const int num_directions
)
{
    const Board *board = game->board;

    Piece *selected_piece = get_piece_at(board, piece_location);
    if (!selected_piece) return false;

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

            // check if move is in bounds
            if (!in_bounds(board, row, col)) break;

            Position to = { .col = col, .row = row };

            // select piece on destination
            Piece *target = get_piece_at(board, to);

            // check for friendly pieces
            if (target && selected_piece->colour == target->colour) break;
            
            // append (only options are empty square or enemy piece)
            if (!append_normal_move(board, piece_location, to, moves_out)) return false;

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
static bool generate_bishop_moves(
    const GameState *game,
    Position piece_location,
    MoveList *moves_out
)
{
    static const int d[4][2] = { {1, 1}, {1, -1}, {-1, -1}, {-1, 1} };
    
    return generate_sliding_moves(game, piece_location, moves_out, d, 4);
}

static bool generate_rook_moves(
    const GameState *game,
    Position piece_location,
    MoveList *moves_out
)
{
    static const int d[4][2] = { {1, 0}, {0, -1}, {-1, 0}, {0, 1} };

    return generate_sliding_moves(game, piece_location, moves_out, d, 4);
}

static bool generate_queen_moves(
    const GameState *game,
    Position piece_location,
    MoveList *moves_out
)
{
    // combination of bishop and rook directions
    static const int d[8][2] = {
        {1, 1}, {1, -1}, {-1, -1}, {-1, 1},
        {1, 0}, {0, -1}, {-1, 0}, {0, 1}
    };

    return generate_sliding_moves(game, piece_location, moves_out, d, 8);
}

// avoids having to use in_bounds() and get_piece_at() afterwards
static bool is_empty_square(const Board *board, Position position)
{
    return in_bounds(board, position.row, position.col) && get_piece_at(board, position) == NULL;
}