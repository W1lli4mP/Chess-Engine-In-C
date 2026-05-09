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

bool remove_piece_at(Board *board, Position piece_pos)
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

bool apply_move(Board *board, Move move)
{
    if (!valid_move(board, move)) return false; // should handle legal moves in the future

    Piece *piece = board->grid[move.from.row][move.from.col];
    Piece *target = board->grid[move.to.row][move.to.col];

    // handle pawn promotions
    if (move.is_promotion)
    {
        piece->type = move.promotion;
        piece->sprite = find_sprite(piece->type, piece->colour);
    }

    //* cannot test yet
    // // handle castling
    // if (move.is_castle_kingside)
    // {
    //     // // move king
    //     // board->grid[move.to.row][move.to.col] = piece;
    //     //? king is already moved in code below (change later to better clarity maybe)

    //     // move rook
    //     Piece *rook = board->grid[move.to.row][board->width];
    //     board->grid[move.to.row][move.to.col - 1] = rook;
    //     board->grid[move.to.row][board->width] = NULL;
    // }

    // move piece from original position to destination
    board->grid[move.to.row][move.to.col] = piece;

    //! can use is_capture from Move
    //! captures should not happen if is_capture is false
    // if there was a piece on the destination (aka a capture), destroy/free it
    if (target) destroy_piece(target);

    // clear original position
    board->grid[move.from.row][move.from.col] = NULL;

    return true;
}

// apply_move() but not destructive (no freeing)
bool simulate_move(Board *board, Move move)
{
    if (!valid_move(board, move)) return false;

    Piece *piece = board->grid[move.from.row][move.from.col];

    // move piece from original position to destination
    board->grid[move.to.row][move.to.col] = piece;

    // clear original position
    board->grid[move.from.row][move.from.col] = NULL;

    return true;
}

// ! redundant for now
bool undo_move(Board *board, Move move)
{
    Piece *piece = board->grid[move.to.row][move.to.col];
    // TODO: handle captures later

    // move piece from destination to original position
    board->grid[move.from.row][move.from.col] = piece;

    // clear original position
    // TODO: restore captured pieces here
    board->grid[move.to.row][move.to.col] = NULL;

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

void print_board(const Board *board, int white_pov)
{
    int row_start, row_end, row_step;
    int col_start, col_end, col_step;

    // setup for loop conditions based on inverted pov
    if (white_pov)
    {
        row_start = board->height - 1; row_end = -1; row_step = -1;
        col_start = 0; col_end = board->width; col_step = 1;
    }
    else
    {
        row_start = 0; row_end = board->height; row_step = 1;
        col_start = 0; col_end = board->width; col_step = 1;
    }

    // print using new conditions
    for (int row = row_start; row != row_end; row += row_step)
    {
        for (int col = col_start; col != col_end; col += col_step)
        {
            // make sure theres a piece on the square before using print_piece
            if (!board->grid[row][col])
                printf(" ");
            else
                print_piece(board->grid[row][col]);
            printf(" ");
        }
        putchar('\n'); // faster than printf()
    }
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