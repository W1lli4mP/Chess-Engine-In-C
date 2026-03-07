#include "san_resolve.h"

// helper for implicit pawn promotions
static bool is_pawn_promotion_rank(const Piece *piece, Position to)
{
    if (!piece || piece->type != TYPE_PAWN) return false;
    if (piece->colour == COLOUR_WHITE && to.row == 7) return true;
    if (piece->colour == COLOUR_BLACK && to.row == 0) return true;
    return false;
}

/*
TODO:
handle castling
handle promotions
handle san suffix (checks/mates)
*/
ResolveStatus resolve_san(const Board *board, San san, Move *move_out)
{
    /*
        pipeline:
        - iterate through all pieces
        - check if SAN move is in the list of valid moves
        - if SAN move is not an ordinary move (captures, castling, promotions), check for them too

        handling captures:
        - must be a legal move
        - enemy piece must be on the SAN's destination
    */

    ResolveStatus current_status = RESOLVE_ILLEGAL;
    for (int row = 0; row < board->height; row++)
    {
        for (int col = 0; col < board->width; col++)
        {
            // check for disambiguation hints from SAN
            if (san.from_col != -1 && san.from_col != col) continue;
            if (san.from_row != -1 && san.from_row != row) continue;

            // iterate through all squares on the board
            Position from = { .row = row, .col = col };
            
            // find valid piece at current square
            Piece *selected_piece = get_piece_at(board, from);
            if (!selected_piece || selected_piece->type != san.piece) continue;

            // promotion moves imply the piece is a pawn
            if (san.is_promotion && selected_piece->type != TYPE_PAWN) continue;

            // check if the selected piece can go to the SAN's square
            PositionList move_list = {0}; // set count=0 and moves to be 0
            if (!generate_legal_moves(board, from, &move_list)) continue;

            // check if the SAN's move is in the list of all possible moves
            for (int i = 0; i < move_list.count; i++)
            {
                // select destination square
                Position to = move_list.moves[i];

                // if there's a match, process it
                if (san.to.row == to.row && san.to.col == to.col)
                {
                    // handle captures
                    if (san.is_capture)
                    {
                        // don't process move if there is no valid piece (enemy) to capture
                        Piece *enemy = get_piece_at(board, to);
                        if (!enemy) continue;
                    }

                    // handle promotions (NOTE: they have lower priority than captures)
                    if (san.is_promotion || is_pawn_promotion_rank(selected_piece, to))
                    {
                        move_out->is_promotion = true;
                        move_out->promotion = (san.promotion == TYPE_NONE) ? TYPE_QUEEN : san.promotion; // default to queens in the case of implicit SAN promotions
                        if (san.promotion == TYPE_NONE) puts("none");
                        if (san.promotion == TYPE_PAWN) puts("pawn");
                    }

                    // if the status has previously been marked as OK, then there must be more than one piece that can access this move
                    if (current_status == RESOLVE_OK) return RESOLVE_AMBIGUOUS;
                    
                    // approve status and update move's source square
                    current_status = RESOLVE_OK;
                    move_out->from = from;
                    continue;
                }
            }
        }
    }
    move_out->piece = san.piece;
    move_out->to = san.to;

    return current_status;
}