#include "game_state.h"

GameState *create_game_state()
{
    GameState *game = malloc(sizeof *game);
    if (!game) return NULL;
    
    game->board = create_starting_board();
    if (!game->board)
    {
        free(game);
        return NULL;
    }

    game->side_to_move = COLOUR_WHITE;

    game->castling_rights.white_can_castle_kingside = true;
    game->castling_rights.white_can_castle_queenside = true;
    game->castling_rights.black_can_castle_kingside = true;
    game->castling_rights.black_can_castle_queenside = true;

    game->has_en_passant_target = false;
    game->en_passant_target = (Position) { .row = -1, .col = -1 };

    game->halfmove_clock = 0;
    game->fullmove_number = 1;

    game->position_history_count = 0;

    return game;
}

GameState *create_starting_game_state()
{
    return create_game_state();
}

void destroy_game_state(GameState *game)
{
    if (!game) return;

    destroy_board(game->board);
    free(game);
}

void switch_side_to_move(GameState *game)
{
    if (!game) return;

    game->side_to_move = (game->side_to_move == COLOUR_WHITE) ? COLOUR_BLACK : COLOUR_WHITE;
    
    if (game->side_to_move == COLOUR_WHITE) game->fullmove_number++;
}

bool reset_game_state(GameState *game)
{
    if (!game) return false;

    Board *new_board = create_starting_board();
    if (!new_board) return false;

    destroy_board(game->board);
    game->board = new_board;

    game->side_to_move = COLOUR_WHITE;

    game->castling_rights.white_can_castle_kingside = true;
    game->castling_rights.white_can_castle_queenside = true;
    game->castling_rights.black_can_castle_kingside = true;
    game->castling_rights.black_can_castle_queenside = true;

    game->has_en_passant_target = false;
    game->en_passant_target = (Position) { .row = -1, .col = -1 };

    game->halfmove_clock = 0;
    game->fullmove_number = 1;

    game->position_history_count = 0;

    return true;
}