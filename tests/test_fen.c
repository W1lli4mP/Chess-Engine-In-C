#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "fen_parser.h"

#define FEN_CASES_FILE "tests/data/fen_cases.txt"
#define MAX_LINE_LEN 256

static void print_result(const char *test_id, const char *fen, bool passed, int err_pos, bool expected_valid)
{
    printf("Test: %s\n", test_id);
    printf("FEN: %s\n", fen);
    printf("Expected: %s\n", expected_valid ? "valid" : "invalid");

    if (passed)
        puts("Result: PASS");
    else
        printf("Result: FAIL (err_pos = %d)\n", err_pos);

    puts("------------------------");
}

static bool test_valid_fen(const char *test_id, const char *fen)
{
    // initialise new game state
    GameState *game = create_game_state();

    if (!game)
    {
        puts("Failed to allocate game state");
        destroy_game_state(game);
        return false;
    }

    // make the game state's board empty
    game->board = initialise_empty_board();

    if (!game->board)
    {
        puts("Failed to allocate board");
        destroy_game_state(game);
        return false;
    }

    // set error position
    int err_pos = -1;

    bool ok = load_fen(game, fen, &err_pos);

    print_result(test_id, fen, ok, err_pos, true);

    if (ok)
    {
        print_board(game->board, 1);
        printf("Side to move: %s\n", (game->side_to_move == COLOUR_WHITE) ? "white" : "black");
        printf("Castling: K=%d Q=%d k=%d q=%d\n",
            game->castling_rights.white_can_castle_kingside,
            game->castling_rights.white_can_castle_queenside,
            game->castling_rights.black_can_castle_kingside,
            game->castling_rights.black_can_castle_queenside
        );

        if (game->has_en_passant_target)
        {
            printf(
                "En passant: %c%c\n",
                'a' + game->en_passant_target.col,
                '1' + game->en_passant_target.row
            );
        }
        else
        {
            puts("En passant: -");
        }

        printf("Halfmove: %d\n", game->halfmove_clock);
        printf("Fullmove %d\n", game->fullmove_number);

        puts("------------------------");
    }

    destroy_game_state(game);
    return ok;
}

static bool test_invalid_fen(const char *test_id, const char *fen)
{
    // initialise new game state
    GameState *game = create_game_state();

    if (!game)
    {
        puts("Failed to allocate game state");
        destroy_game_state(game);
        return false;
    }

    // make the game state's board empty
    game->board = initialise_empty_board();

    if (!game->board)
    {
        puts("Failed to allocate board");
        destroy_game_state(game);
        return false;
    }

    // set error position
    int err_pos = -1;

    bool ok = load_fen(game, fen, &err_pos);

    print_result(test_id, fen, !ok, err_pos, false);

    destroy_game_state(game);
    return !ok;
}

int main()
{
    puts("------------------------");

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

        char *test_id = line;

        // 1st separator
        char *sep1 = strchr(line, '|');

        if (!sep1)
        {
            printf("Malformed test line: %s\n", line);
            continue;
        }

        *sep1 = '\0';

        char *expected = sep1 + 1;
        
        // 2nd separator
        char *sep2 = strchr(expected, '|');

        if (!sep2)
        {
            printf("Malformed test line: %s\n", line);
            continue;
        }

        *sep2 = '\0';

        char *fen = sep2 + 1;

        // process test case
        if (strcmp(expected, "valid") == 0)
        {
            total++;
            if (test_valid_fen(test_id, fen)) passed++;
        }
        else if (strcmp(expected, "invalid") == 0)
        {
            total++;
            if (test_invalid_fen(test_id, fen)) passed++;
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