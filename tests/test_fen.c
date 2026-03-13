#include <stdio.h>
#include <stdbool.h>
#include "board.h"
#include "fen_parser.h"

static void print_result(const char *fen, bool passed, int err_pos, bool expected_valid)
{
    printf("FEN: %s\n", fen);
    printf("Expected: %s\n", expected_valid ? "valid" : "invalid");

    if (passed)
        puts("Result: PASS");
    else
        printf("Result: FAIL (err_pos = %d)\n", err_pos);

    puts("------------------------");
}

static bool test_valid_fen(const char *fen)
{
    Board *board = initialise_empty_board();

    if (!board)
    {
        puts("Failed to allocate board");
        return false;
    }

    FenMeta meta;
    int err_pos = -1;

    bool ok = load_fen(board, fen, &meta, &err_pos);

    print_result(fen, ok, err_pos, true);

    if (ok)
    {
        print_board(board, 1);
        printf("Side to move: %s\n", meta.white_to_move ? "white" : "black");
        printf("Castling: K=%d Q=%d k=%d q=%d\n",
               board->white_can_castle_kingside,
               board->white_can_castle_queenside,
               board->black_can_castle_kingside,
               board->black_can_castle_queenside);

        if (meta.has_en_passant)
            printf("En passant: %c%c\n",
                   'a' + meta.en_passant.col,
                   '1' + meta.en_passant.row);
        else
            puts("En passant: -");
        
        printf("Halfmove: %d\n", meta.halfmove_clock);
        printf("Fullmove: %d\n", meta.fullmove_number);
        puts("------------------------");
    }

    destroy_board(board);
    return ok;
}

static bool test_invalid_fen(const char *fen)
{
    Board *board = initialise_empty_board();
    
    if (!board)
    {
        puts("Failed to allocate board");
        return false;
    }

    FenMeta meta;
    int err_pos = -1;

    bool ok = load_fen(board, fen, &meta, &err_pos);

    print_result(fen, !ok, err_pos, false);

    destroy_board(board);
    return !ok;
}

int main()
{
    int passed = 0;
    int total = 0;

    total++; if (test_valid_fen("8/8/8/8/8/8/8/8 w - - 0 1")) passed++;
    total++; if (test_valid_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")) passed++;
    total++; if (test_valid_fen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1")) passed++;

    total++; if (test_invalid_fen("8/8/8/8/8/8/8 w - - 0 1")) passed++;
    total++; if (test_invalid_fen("8/8/8/8/8/8/8/8 x - - 0 1")) passed++;
    total++; if (test_invalid_fen("8/8/8/8/8/8/8/8 w - - 0 0")) passed++;

    printf("Passed %d/%d tests\n", passed, total);

    return passed != total;
}