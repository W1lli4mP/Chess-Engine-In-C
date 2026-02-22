#include "san_resolve.h"

/*
TODO:
handle castling
handle promotions
handle san suffix (checks/mates)
handle is capture
*/
ResolveStatus resolve_san(const Board *board, San san, Move *move_out)
{
    ResolveStatus current_status = RESOLVE_ILLEGAL;
    PieceType piece_type_target = san.piece;
    for (int row = 0; row < board->height; row++)
    {
        for (int col = 0; col < board->width; col++)
        {
            // check for disambiguation hints from SAN
            if (san.from_col != -1 && san.from_col != col) continue;
            if (san.from_row != -1 && san.from_row != row) continue;

            // iterate through all positions on the board
            Position current_position = { .row = row, .col = col };
            
            // find piece at current observed position
            Piece *selected_piece = get_piece_at(board, current_position);
            
            // skip if selected piece does not exist or wrong type of piece
            if (!selected_piece || selected_piece->type != piece_type_target) continue;

            // otherwise, check if the selected piece can go to the SAN's desired destination
            PositionList move_list = {0}; // set count=0 and moves to be 0
            if (!generate_pseudo_legal_moves(board, current_position, &move_list)) continue;

            // check if the SAN's desired move is in the list of possible moves
            for (int i = 0; i < move_list.count; i++)
            {
                // if there's a match, break
                if (san.to.row == move_list.moves[i].row && san.to.col == move_list.moves[i].col)
                {
                    // TEMPORARY: link from square
                    move_out->from = current_position;

                    // if the status has previously been marked as OK, then there must be more than one piece that can access this move
                    if (current_status == RESOLVE_OK)
                    {
                        current_status = RESOLVE_AMBIGUOUS;
                        break;
                    }
                    current_status = RESOLVE_OK;
                    break;
                }
            }
        }
    }
    move_out->piece = san.piece;
    move_out->to = san.to;
    // move_out->from = ??;

    return current_status;
}
