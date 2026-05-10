#include <string.h>

#include "position_key.h"
#include "board.h"

static char piece_to_fen_char(const Piece *piece)
{
    if (!piece) return '\0';

    switch (piece->type)
    {
        case TYPE_PAWN: return piece->colour == COLOUR_WHITE ? 'P' : 'p';
        case TYPE_ROOK: return piece->colour == COLOUR_WHITE ? 'R' : 'r';
        case TYPE_KNIGHT: return piece->colour == COLOUR_WHITE ? 'N' : 'n';
        case TYPE_BISHOP: return piece->colour == COLOUR_WHITE ? 'B' : 'b';
        case TYPE_QUEEN: return piece->colour == COLOUR_WHITE ? 'Q' : 'q';
        case TYPE_KING: return piece->colour == COLOUR_WHITE ? 'K' : 'k';
        default: return '\0';
    }
}

static bool append_char(char *out, size_t out_size, size_t *len, char c)
{
    if (*len + 1 >= out_size) return false;

    out[*len] = c;
    (*len)++;
    out[*len] = '\0';

    return true;
}

// constructs a position key in FEN style
// TODO: extend to zobrist hashing in the future
bool create_position_key(const GameState *game, PositionKey *key_out)
{
    if (!game || !game->board || !key_out) return false;

    char *out = key_out->text;
    size_t len = 0;
    out[0] = '\0';

    // piece placement (in FEN style)
    for (int row = BOARD_SIZE - 1; row >= 0; row--)
    {
        int empty_count = 0;

        for (int col = 0; col < BOARD_SIZE; col++)
        {
            Position pos = { .row = row, .col = col };
            Piece *piece = get_piece_at(game->board, pos);
            
            if (!piece)
            {
                empty_count++;
                continue;
            }

            // append empty squares
            if (empty_count > 0)
            {
                if (!append_char(out, POSITION_KEY_LEN, &len, (char) ('0' + empty_count))) return false;
                empty_count = 0;
            }

            char piece_char = piece_to_fen_char(piece);
            if (!piece_char) return false;

            if (!append_char(out, POSITION_KEY_LEN, &len, piece_char)) return false;
        }

        if (empty_count > 0)
        {
            if (!append_char(out, POSITION_KEY_LEN, &len, (char) ('0' + empty_count))) return false;
        }

        if (row > 0)
        {
            if (!append_char(out, POSITION_KEY_LEN, &len, '/')) return false;
        }
    }

    // side to move
    if (!append_char(out, POSITION_KEY_LEN, &len, ' ')) return false;
    if (!append_char(out, POSITION_KEY_LEN, &len, game->side_to_move == COLOUR_WHITE ? 'w' : 'b')) return false;

    // castling rights
    if (!append_char(out, POSITION_KEY_LEN, &len, ' ')) return false;

    bool any_castling = false;

    if (game->castling_rights.white_can_castle_kingside)
    {
        if (!append_char(out, POSITION_KEY_LEN, &len, 'K')) return false;
        any_castling = true;
    }

    if (game->castling_rights.white_can_castle_queenside)
    {
        if (!append_char(out, POSITION_KEY_LEN, &len, 'Q')) return false;
        any_castling = true;
    }

    if (game->castling_rights.black_can_castle_kingside)
    {
        if (!append_char(out, POSITION_KEY_LEN, &len, 'k')) return false;
        any_castling = true;
    }

    if (game->castling_rights.black_can_castle_queenside)
    {
        if (!append_char(out, POSITION_KEY_LEN, &len, 'q')) return false;
        any_castling = true;
    }

    if (!any_castling)
    {
        if (!append_char(out, POSITION_KEY_LEN, &len, '-')) return false;
    }

    // en passant target
    if (!append_char(out, POSITION_KEY_LEN, &len, ' ')) return false;

    if (game->has_en_passant_target)
    {
        char file = (char)('a' + game->en_passant_target.col);
        char rank = (char)('1' + game->en_passant_target.row);

        if (!append_char(out, POSITION_KEY_LEN, &len, file)) return false;
        if (!append_char(out, POSITION_KEY_LEN, &len, rank)) return false;
    }
    else
    {
        if (!append_char(out, POSITION_KEY_LEN, &len, '-')) return false;
    }

    return true;
}

bool position_keys_equal(const PositionKey *a, const PositionKey *b)
{
    if (!a || !b) return false;
    return strcmp(a->text, b->text) == 0;
}