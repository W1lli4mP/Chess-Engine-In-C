#include <string.h>

#include "uci.h"
#include "move_apply.h"

static void square_to_text(Square square, char out[3]);

static bool parse_square(const char *text, Square *square_out);

static bool parse_promotion_piece(char c, PieceType *promotion_out);

static void square_to_text(Square square, char out[3])
{
    out[0] = (char) ('a' + square.col);
    out[1] = (char) ('1' + square.row);
    out[2] = '\0';
}

static bool parse_square(const char *text, Square *square_out)
{
    if (!text || !square_out) return false;

    // squares must always be denoted as two chars: <col> | <row>
    if (strlen(text) != 2) return false;

    char col = text[0];
    char row = text[1];

    if (col < 'a' || col > 'h') return false;

    if (row < '1' || row > '8') return false;

    square_out->col = col - 'a';
    square_out->row = row - '1';

    return true;
}

static bool parse_promotion_piece(char c, PieceType *promotion_out)
{
    if (!promotion_out) return false;

    switch (c)
    {
        case 'Q': *promotion_out = TYPE_QUEEN; return true;
        case 'R': *promotion_out = TYPE_ROOK; return true;
        case 'B': *promotion_out = TYPE_BISHOP; return true;
        case 'N': *promotion_out = TYPE_KNIGHT; return true;
        default: return false;
    }
}

bool parse_uci_move(GameState *game, const char *text, Move *move_out)
{
    if (!game || !text || !move_out) return false;

    if (strlen(text) < 4 || strlen(text) > 5) return false;

    // partition text
    char from_text[3] = { text[0], text[1], '\0' };

    char to_text[3] = { text[2], text[3], '\0' };

    char promotion_text = { text[4] ? (strlen(text) == 5) : '\0', '\0'};

    // parse text
    Square from;
    
    if (!parse_square(from_text, &from)) return false;

    Square to;

    if (!parse_square(to_text + 2, &to)) return false;

    // parse promotion
    PieceType promotion;
    if (!parse_promotion_piece(promotion_text, &promotion)) return false;

    //! simulate move and extract info to populate Move
}


bool move_to_uci(Move move, char out[6])
{
    if (!out) return false;

    char from[3];
    char to[3];

    square_to_text(move.from, from);
    square_to_text(move.to, to);

    char promotion = '\0';

    if (move.is_promotion)
    {
        switch (move.promotion)
        {
            case TYPE_QUEEN: promotion = 'q'; break;
            case TYPE_ROOK: promotion = 'r'; break;
            case TYPE_BISHOP: promotion = 'b'; break;
            case TYPE_KNIGHT: promotion = 'n'; break;
            default: return false;
        }
    }

    strncpy(out, from, 2);
    strncpy(out + 2, to, 2);
    out[4] = promotion;
    out[5] = '\0';

    return true;
}