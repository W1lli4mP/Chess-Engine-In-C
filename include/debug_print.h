#ifndef DEBUG_PRINT_H
#define DEBUG_PRINT_H

#include <stdio.h>

#include "board.h"
#include "move.h"
#include "san.h"
#include "san_resolve.h"

#define WHITE_VIEW 1
#define BLACK_VIEW 0

void debug_print_piece(const Piece *piece);
void debug_print_board(const Board *board, int white_pov);
void debug_print_move(Move move);
void debug_print_san(San san);
void debug_print_resolve_status(ResolveStatus status);
void debug_print_engine_move_score(Move move, int score);
void debug_print_engine_best_move(Move move, int score);

// macro for engine debug prints
#ifdef ENGINE_DEBUG
void debug_print_engine_move_score(Move move, int score);
void debug_print_engine_best_move(Move move, int score);
#endif

#endif