#include "san.h"

San *create_san(void)
{
    San *san = malloc(sizeof *san);
    if (!san) return NULL;

    san->piece = TYPE_NONE;
    Position to = { .col = -1, .row = -1 };
    san->to = to;
    san->is_capture = false;

    // castling intent
    san->is_castle_kingside = false;
    san->is_castle_queenside = false;

    // disambiguation hints
    san->from_col = -1;
    san->from_row = -1;

    // promotion
    san->is_promotion = false;
    san->promotion = TYPE_NONE;

    san->suffix = SAN_SUFFIX_NONE;
    return san;
}

bool destroy_san(San *san)
{
    if (!san) return false;

    free(san);

    return true;
}