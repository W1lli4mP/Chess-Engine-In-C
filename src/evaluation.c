#include "evaluation.h"

// TODO: integrate NNUE later
// static evaluation (in centipawns)
static int piece_value(PieceType type)
{
    switch (type)
    {
        case TYPE_PAWN: return 100;
        case TYPE_KNIGHT: return 320;
        case TYPE_BISHOP: return 330;
        case TYPE_ROOK: return 500;
        case TYPE_QUEEN: return 900;
        case TYPE_KING: return 0;
        default: return 0;
    }
}

int evaluate_position(const GameState *game)
{
    if (!game || !game->board) return 0;

    int score = 0;

    // iterate through all pieces and sum up (evaluate) their piece values
    // white has a positive influence on the score
    // black has a negative influence on the score
    for (int row = 0; row < game->board->height; row++)
    {
        for (int col = 0; col < game->board->width; col++)
        {
            Position pos = { .row = row, .col = col };
            Piece *piece = get_piece_at(game->board, pos);

            if (!piece) continue;

            int value = piece_value(piece->type);

            if (piece->colour == COLOUR_WHITE)
                score += value;
            else if (piece->colour == COLOUR_BLACK)
                score -= value;
        }
    }

    return score;
}