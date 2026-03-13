#include "fen_parser.h"

static PieceType char_to_piece_type(char c);
static bool is_piece_char(char c);

static bool parse_board_field(const char **p, FenPosition *fen_out, int *err_pos);
static bool parse_side_to_move(const char **p, FenPosition *fen_out, int *err_pos);
static bool parse_castling(const char **p, FenPosition *fen_out, int *err_pos);
static bool parse_en_passant(const char **p, FenPosition *fen_out, int *err_pos);


bool parse_fen(const char *fen, int *err_pos, FenPosition *fen_out)
{
    if (!fen || !fen_out)
    {
        if (err_pos) *err_pos = 0;
        return false;
    }

    // add actual parsing logic

    return true;
}