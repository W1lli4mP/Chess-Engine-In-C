#include "rules.h"

Position get_king_position(Board *b, char colour)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            Position pos = {i, j};
            Piece *p = get_piece_at(b, pos);
            if (p->type == 'k' && p->colour == colour)
            {
                Position kp = {i, j};
                return kp;
            }
        }
    }
}

int is_in_check(Board *b, char colour)
{
    for (int x; x < 8; x++)
    {
        for (int y; y < 8; y++)
        {
            // scan through potential enemies
            Position pos = {x, y};
            Piece *enemy = get_piece_at(b, pos);
            
            // if not an enemy, skip
            if (!(is_enemy(b, enemy, pos)))
            {
                continue;
            }
            
            // calculate all of the enemy's moves
            Position *moves;
            int count = generate_pseudo_legal_moves(b, enemy, moves);
            
            // compare each move to the king's position
            Position kp = get_king_position(b, colour);

            for (int i; i < count; i++)
            {
                if (moves[i].row == kp.row && moves[i].col == kp.col) // check if one of the moves align
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

int is_checkmate(Board *b, char colour)
{
    if (is_in_check(b, colour))
    {
        // calculate if king has any legal moves
        Position kp = get_king_position(b, colour);
        Piece *king = get_piece_at(b, kp);
        Position *legal_moves; // buffer - not really needed
        int num_moves = generate_pseudo_legal_moves(b, king, legal_moves);
    
        if (num_moves == 0)
        {
            return 1;
        }
    }
    return 0;
}