#include <string.h>

#include "uci.h"
#include "move_gen.h"

static void square_to_text(Square square, char out[3]);

static bool parse_square(const char *text, Square *square_out);

static bool parse_uci_promotion(char c, PieceType *promotion_out);

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

static bool parse_uci_promotion(char c, PieceType *promotion_out)
{
    if (!promotion_out) return false;

    // UCI promotion is in lower case
    switch (c)
    {
        case 'q': *promotion_out = TYPE_QUEEN; return true;
        case 'r': *promotion_out = TYPE_ROOK; return true;
        case 'b': *promotion_out = TYPE_BISHOP; return true;
        case 'n': *promotion_out = TYPE_KNIGHT; return true;
        default: return false;
    }
}

bool parse_uci_move(GameState *game, const char *text, Move *move_out)
{
    if (!game || !text || !move_out) return false;

    size_t len = strlen(text);
    if (len != 4 && len != 5) return false;

    // partition text
    char from_text[3] = { text[0], text[1], '\0' };

    char to_text[3] = { text[2], text[3], '\0' };

    char promotion_text = '\0';

    if (len == 5) promotion_text = text[4];

    // parse text
    Square from;
    
    if (!parse_square(from_text, &from)) return false;

    Square to;

    if (!parse_square(to_text, &to)) return false;

    // parse promotion
    // flag for validating when retrieving Move
    bool has_promotion = false;
    PieceType promotion = TYPE_NONE;

    if (len == 5)
    {
        has_promotion = true;

        if (!parse_promotion_piece(promotion_text, &promotion)) return false;
    }

    // generate moves and find a match
    MoveList moves = {0};
    if (!generate_legal_moves(game, from, &moves)) return false;

    for (int i = 0; i < moves.count; i++)
    {
        Move move = moves.moves[i];

        if (!squares_equal(move.to, to)) continue;

        if (has_promotion)
        {
            if (!move.is_promotion) continue;

            if (move.promotion != promotion) continue;
        }
        else
        {
            // skip non-promoting moves if promotion is expected by the UCI
            if (move.is_promotion) continue;
        }

        *move_out = move;
        return true;
    }

    return false;
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