#ifndef FEN_PARSER_H
#define FEN_PARSER_H
#include <stdbool.h>
#include "board.h"
#include "piece.h"
#include "position.h"

typedef struct
{
    Board board;
    bool white_to_move;
    bool castle_K;
    bool castle_Q;
    bool castle_k;
    bool castle_q;
    bool has_en_passant;
    Position en_passant;
} FenPosition;

bool parse_fen(const char *fen, int *err_pos, FenPosition *fen_out);

#endif