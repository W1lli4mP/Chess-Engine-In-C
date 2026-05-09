#include "board.h"

Board *initialise_board()
{
    Board *new_board = malloc(sizeof *new_board);
    if (!new_board) return NULL;

    new_board->height = 8;
    new_board->width = 8;

    for (int row = 0; row < new_board->height; row++)
    {
        for (int col = 0; col < new_board->width; col++)
        {
            // decide piece
            PieceType type = TYPE_NONE;
            Colour colour = COLOUR_NONE;


            if (row == 0 || row == 1) colour = COLOUR_WHITE;
            if (row == new_board->height - 1 || row == new_board->height - 2) colour = COLOUR_BLACK;

            // first row
            if (col == 0 || col == new_board->width - 1) type = TYPE_ROOK;
            if (col == 1 || col == new_board->width - 2) type = TYPE_KNIGHT;
            if (col == 2 || col == new_board->width - 3) type = TYPE_BISHOP;
            if (col == 3) type = TYPE_QUEEN;
            if (col == 4) type = TYPE_KING;

            // second row
            if (row == 1 || row == new_board->height - 2) type = TYPE_PAWN;

            if (type == TYPE_NONE || colour == COLOUR_NONE)
            {
                new_board->grid[row][col] = NULL;
            }
            else
            {
                Piece *curr_piece = create_piece(type, colour);
                if (!curr_piece)
                {
                    destroy_board(new_board);
                    return NULL;
                }
                
                new_board->grid[row][col] = curr_piece;
            }
        }
    }

    return new_board;
}

Piece *get_piece_at(const Board *board, Position piece_pos)
{
    if (!board) return NULL;

    if (piece_pos.row < 0 || piece_pos.row >= board->height || piece_pos.col < 0 || piece_pos.col >= board->width) return NULL;

    Piece *piece = board->grid[piece_pos.row][piece_pos.col];
    return piece;
}

// allows piece to be NULL if removing a piece
bool set_piece_at(Board *board, Position piece_pos, Piece *piece)
{
    if (!board) return false;

    if (piece_pos.row < 0 || piece_pos.row >= board->height || piece_pos.col < 0 || piece_pos.col >= board->width) return false;

    board->grid[piece_pos.row][piece_pos.col] = piece;
    return true;
}

bool destroy_piece_at(Board *board, Position piece_pos)
{
    // retrieve piece before removing it from the board
    Piece *piece = get_piece_at(board, piece_pos);
    
    if (!piece) return false;

    // set board position to NULL to "remove" the old piece
    if (!set_piece_at(board, piece_pos, NULL)) return false;

    // free the piece from memory
    destroy_piece(piece);
    return true;
}

bool destroy_board(Board *board)
{
    if (!board) return false;

    clear_board(board);
    free(board);

    return true;
}

// extendable helper for future changes
bool valid_move(Board *board, Move move)
{
    // board boundaries
    if (!(move.from.col >= 0 && move.from.col < board->width &&
            move.from.row >= 0 && move.from.row < board->height &&
            move.to.col >= 0 && move.to.col < board->width &&
            move.to.row >= 0 && move.to.row < board->height)) return false;

    // find piece
    Piece *piece = board->grid[move.from.row][move.from.col];
    if (!piece) return false;

    // TODO: check for legal moves in another function
    return true;
}

bool in_bounds(const Board *board, int row, int col)
{
    if (!board) return false;
    return col >= 0 && col < board->width && row >= 0 && row < board->height;
}

Board *initialise_empty_board()
{
    Board *new_board = malloc(sizeof *new_board);
    if (!new_board) return NULL;

    new_board->height = 8;
    new_board->width = 8;

    for (int row = 0; row < new_board->height; row++)
    {
        for (int col = 0; col < new_board->width; col++)
        {
            new_board->grid[row][col] = NULL;
        }
    }

    return new_board;
}

bool clear_board(Board *board)
{
    if (!board) return false;

    for (int row = 0; row < board->height; row++)
    {
        for (int col = 0; col < board->width; col++)
        {
            if (board->grid[row][col])
            {
                destroy_piece(board->grid[row][col]);
                board->grid[row][col] = NULL;
            }
        }
    }

    return true;
}