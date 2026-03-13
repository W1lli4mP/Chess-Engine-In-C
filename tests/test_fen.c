#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "fen_parser.h"

#define FEN_CASES_FILE "tests/data/fen_cases.txt"
#define MAX_LINE_LEN 256

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
    FILE *fp = fopen(FEN_CASES_FILE, "r");
    if (!fp)
    {
        perror("Failed to open FEN test file");
        return 1;
    }

    int passed = 0;
    int total = 0;

    char line[MAX_LINE_LEN];

    while (fgets(line, sizeof line, fp))
    {
        size_t len = strcspn(line, "\r\n");
        line[len] = '\0';

        // skip blank lines and comments
        if (line[0] == '\0' || line[0] == '#') continue;

        // separator
        char *sep = strchr(line, '|');

        if (!sep)
        {
            printf("Malformed test line: %s\n", line);
            continue;
        }

        *sep = '\0';

        char *expected = line;
        char *fen = sep + 1;

        if (strcmp(expected, "valid") == 0)
        {
            total++;
            if (test_valid_fen(fen)) passed++;
        }
        else if (strcmp(expected, "invalid") == 0)
        {
            total++;
            if (test_invalid_fen(fen)) passed++;
        }
        else
        {
            printf("Unknown test type: %s\n", expected);
        }
    }

    fclose(fp);

    printf("Passed %d/%d tests\n", passed, total);

    return passed != total;
}