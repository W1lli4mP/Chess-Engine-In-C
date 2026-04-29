#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdlib.h>
#include <stdbool.h>
#include "board.h"
#include "piece.h"
#include "position.h"

typedef struct
{
    bool white_can_castle_kingside;
    bool white_can_castle_queenside;
    bool black_can_castle_kingside;
    bool black_can_castle_queenside;
} CastlingRights;

typedef struct
{
    Board *board;

    Colour side_to_move;
    
    CastlingRights castling_rights;

    bool has_en_passant_target;
    Position en_passant_target;

    int halfmove_clock;
    int fullmove_number;
} GameState;

GameState *create_game_state();
GameState *create_starting_game_state();

void destroy_game_state();

void switch_side_to_move();

#endif