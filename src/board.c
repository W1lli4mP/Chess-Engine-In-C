#include <stdlib.h>
#include "board.h"

Board *create_starting_board(void)
{
    Board *new_board = create_empty_board();
    if (!new_board) return NULL;

    // better to explicitly define the back rank
    PieceType back_rank[8] = {
        TYPE_ROOK,
        TYPE_KNIGHT,
        TYPE_BISHOP,
        TYPE_QUEEN,
        TYPE_KING,
        TYPE_BISHOP,
        TYPE_KNIGHT,
        TYPE_ROOK
    };

    for (int col = 0; col < BOARD_SIZE; col++)
    {
        // create all pieces for both colours and ranks
        Piece *white_back = create_piece(back_rank[col], COLOUR_WHITE);
        Piece *white_pawn = create_piece(TYPE_PAWN, COLOUR_WHITE);
        Piece *black_pawn = create_piece(TYPE_PAWN, COLOUR_BLACK);
        Piece *black_back = create_piece(back_rank[col], COLOUR_BLACK);

        // if one piece failed to create, exit
        if (!white_back || !white_pawn || !black_pawn || !black_back)
        {
            destroy_piece(white_back);
            destroy_piece(white_pawn);
            destroy_piece(black_pawn);
            destroy_piece(black_back);
            destroy_board(new_board);
            return NULL;
        }

        // place the pieces on the board
        new_board->grid[0][col] = white_back;
        new_board->grid[1][col] = white_pawn;
        new_board->grid[6][col] = black_pawn;
        new_board->grid[7][col] = black_back;
    }

    return new_board;
}

Board *create_empty_board(void)
{
    Board *new_board = malloc(sizeof *new_board);
    if (!new_board) return NULL;

    new_board->height = BOARD_SIZE;
    new_board->width = BOARD_SIZE;

    for (int row = 0; row < new_board->height; row++)
    {
        for (int col = 0; col < new_board->width; col++)
        {
            new_board->grid[row][col] = NULL;
        }
    }

    return new_board;
}

Piece *get_piece_at(const Board *board, Position pos)
{
    if (!board) return NULL;

    if (pos.row < 0 || pos.row >= board->height || pos.col < 0 || pos.col >= board->width) return NULL;

    Piece *piece = board->grid[pos.row][pos.col];
    return piece;
}

// does not destroy any existing piece
// caller is responsible for ensuring no leak/overwrite happens
bool set_piece_at(Board *board, Position pos, Piece *piece)
{
    if (!board) return false;

    if (pos.row < 0 || pos.row >= board->height || pos.col < 0 || pos.col >= board->width) return false;

    board->grid[pos.row][pos.col] = piece;
    return true;
}

// replaces any existing piece at a square, destroying the old one
// Board takes ownership of piece
bool replace_piece_at(Board *board, Position pos, Piece *piece)
{
    if (!board || !in_bounds(board, pos.row, pos.col)) return false;

    if (board->grid[pos.row][pos.col])
    {
        destroy_piece(board->grid[pos.row][pos.col]);
    }

    board->grid[pos.row][pos.col] = piece;
    return true;
}

// destroys and clears the piece at pos
bool destroy_piece_at(Board *board, Position pos)
{
    // retrieve piece before removing it from the board
    Piece *piece = get_piece_at(board, pos);
    
    if (!piece) return false;

    // set board position to NULL to "remove" the old piece
    if (!set_piece_at(board, pos, NULL)) return false;

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

bool is_move_on_board(const Board *board, Move move)
{
    if (!board) return false;

    // board boundaries
    if (!in_bounds(board, move.from.row, move.from.col)) return false;
    if (!in_bounds(board, move.to.row, move.to.col)) return false;

    return get_piece_at(board, move.from) != NULL;
}

bool in_bounds(const Board *board, int row, int col)
{
    if (!board) return false;
    return col >= 0 && col < board->width && row >= 0 && row < board->height;
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