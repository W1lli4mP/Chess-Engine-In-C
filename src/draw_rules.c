#include "draw_rules.h"
#include "board.h"
#include "piece.h"
#include "position_key.h"
#include "square.h"

static bool is_light_square(Square square)
{
    // light squares are always even, vice versa
    return ((square.row + square.col) % 2) == 0;
}

bool is_fifty_move_draw(const GameState *game)
{
    return game && game->halfmove_clock >= 100;
}

bool is_insufficient_material(const GameState *game)
{
    if (!game || !game->board) return false;

    // track minor pieces
    int white_minor_count = 0;
    int black_minor_count = 0;

    int white_bishop_count = 0;
    int black_bishop_count = 0;
    int white_knight_count = 0;
    int black_knight_count = 0;

    // bishops on same coloured squares lead to draws
    int white_bishop_square_colour = -1;
    int black_bishop_square_colour = -1;

    // search through entire board
    for (int row = 0; row < game->board->height; row++)
    {
        for (int col = 0; col < game->board->width; col++)
        {
            Square square = { .row = row, .col = col };
            Piece *piece = get_piece_at(game->board, square);
            
            if (!piece) continue;

            switch (piece->type)
            {
                case TYPE_KING:
                    break;

                case TYPE_BISHOP:
                    if (piece->colour == COLOUR_WHITE)
                    {
                        white_minor_count++;
                        white_bishop_count++;
                        white_bishop_square_colour = is_light_square(square);
                    }
                    else if (piece->colour == COLOUR_BLACK)
                    {
                        black_minor_count++;
                        black_bishop_count++;
                        black_bishop_square_colour = is_light_square(square);
                    }
                    break;

                case TYPE_KNIGHT:
                    if (piece->colour == COLOUR_WHITE)
                    {
                        white_minor_count++;
                        white_knight_count++;
                    }
                    else if (piece->colour == COLOUR_BLACK)
                    {
                        black_minor_count++;
                        black_knight_count++;
                    }
                    break;

                default:
                    // any pawn, rook or queen implies mate is still possible
                    return false;
            }
        }
    }

    int total_minor_count = white_minor_count + black_minor_count;

    // king vs king
    if (total_minor_count == 0) return true;

    // king + bishop or king + knight vs king
    if (total_minor_count == 1) return true;

    // king + bishop vs king + bishop with bishops on same coloured squares
    if (
        total_minor_count == 2 &&
        white_bishop_count == 1 &&
        black_bishop_count == 1 &&
        white_knight_count == 0 &&
        black_knight_count == 0 &&
        white_bishop_square_colour == black_bishop_square_colour
    )
    {
        return true;    
    }

    return false;
}

bool is_threefold_repetition(const GameState *game)
{
    if (!game || game->position_history_count <= 0) return false;

    const PositionKey *current = &game->position_history[game->position_history_count - 1];

    int count = 0;

    for (int i = 0; i < game->position_history_count; i++)
    {
        if (position_keys_equal(&game->position_history[i], current)) count++;
    }

    return count >= 3;
}

bool is_draw(const GameState *game)
{
    return (
        is_fifty_move_draw(game) ||
        is_insufficient_material(game) ||
        is_threefold_repetition(game)
    );
}