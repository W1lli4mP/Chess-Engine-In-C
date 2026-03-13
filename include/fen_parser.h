#ifndef FEN_PARSER_H
#define FEN_PARSER_H
#include <stdbool.h>
#include <ctype.h>
#include "board.h"
#include "piece.h"
#include "position.h"

typedef struct
{
    Board board;
    bool white_to_move;
    bool has_en_passant;
    Position en_passant;
    int halfmove_clock;
    int fullmove_number;
} FenMeta;


bool load_fen(Board *board, const char *fen, FenMeta *meta_out, int *err_pos);

#endif