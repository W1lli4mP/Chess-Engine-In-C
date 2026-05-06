#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "game_state.h"
#include "fen_parser.h"
#include "move_gen.h"

#define MOVE_GEN_CASES_FILE "tests/data/move_gen_cases.txt"
#define MAX_LINE_LEN 256

static void print_move_gen_result(
    const char *test_id,
    bool passed,
    const char *reason
);

static bool run_move_gen_case(
    const char *test_id,
    const char *fen,
    const char *from,
    const char *expected_count,
    const char *expected_moves
);

static bool parse_square(const char *text, Position *position_out);

static bool parse_expected_count(const char *expected_count_text, int *expected_count);

static bool validate_generated_moves(
    MoveList *moves,
    int expected_count,
    const char *expected_moves_text
);

int main()
{
    puts("------------------------");

    FILE *fp = fopen(MOVE_GEN_CASES_FILE, "r");
    if (!fp)
    {
        perror("Failed to open Move Generation test file");
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

        char *fen = sep1 + 1;

        // 2nd separator
        char *sep2 = strchr(fen, '|');

        if (!sep2)
        {
            printf("Malformed test line: %s\n", line);
            continue;
        }

        *sep2 = '\0';

        char *from = sep2 + 1;

        // 3rd separator
        char *sep3 = strchr(from, '|');

        if (!sep3)
        {
            printf("Malformed test line: %s\n", line);
            continue;
        }

        *sep3 = '\0';

        char *expected_count = sep3 + 1;

        // 4th separator
        char *sep4 = strchr(expected_count, '|');

        if (!sep4)
        {
            printf("Malformed test line: %s\n", line);
            continue;
        }

        *sep4 = '\0';

        char *expected_moves = sep4 + 1;
        
        // process test case
        total++;

        if (run_move_gen_case(test_id, fen, from, expected_count, expected_moves)) passed++;
    }

    fclose(fp);

    printf("Passed %d/%d tests\n", passed, total);
    
    return passed != total;
}

//* MAIN HELPER
static bool run_move_gen_case(
    const char *test_id,
    const char *fen,
    const char *from_text,
    const char *expected_count_text,
    const char *expected_moves_text
)
{
    Position from;

    // parse square
    if (!parse_square(from_text, &from))
    {
        print_move_gen_result(test_id, false, "invalid from square");
        return false;
    }

    // parse expected result
    int expected_count;

    if (!parse_expected_count(expected_count_text, &expected_count))
    {
        print_move_gen_result(test_id, false, "invalid expected count");
        return false;
    }

    // setup game state
    GameState *game = create_game_state();

    if (!game)
    {
        print_move_gen_result(test_id, false, "failed to create game state");
        return false;
    }

    // setup empty board state
    game->board = initialise_empty_board();

    if (!game->board)
    {
        destroy_game_state(game);
        print_move_gen_result(test_id, false, "failed to create board");
        return false;
    }

    int err_pos = -1;

    // load FEN
    if (!load_fen(game, fen, &err_pos))
    {
        destroy_game_state(game);
        print_move_gen_result(test_id, false, "failed to load FEN");
        return false;
    }

    // generate legal moves
    MoveList moves = {0};

    if (!generate_legal_moves(game, from, &moves))
    {
        destroy_game_state(game);
        print_move_gen_result(test_id, false, "failed to generate legal moves");
        return false;
    }

    // validate generated legal moves
    bool ok = validate_generated_moves(&moves, expected_count, expected_moves_text);

    // reason is only outputted when failure occurs; no need for a ternary operator
    print_move_gen_result(test_id, ok, "generated moves did not match expected moves");

    destroy_game_state(game);
    return ok;
}

// HELPERS
static void print_move_gen_result(
    const char *test_id,
    bool passed,
    const char *reason
)
{
    printf("Test: %s\n", test_id);
    
    //? could change reason format
    if (passed)
        puts("Result: PASS");
    else
        printf("Result: FAIL (reason = %s)\n", reason);
    
    puts("------------------------");
}

static bool parse_square(const char *text, Position *position_out)
{
    if (!text || !position_out) return false;

    // squares must always be denoted as two chars: <col> | <row>
    if (strlen(text) != 2) return false;

    char col = text[0];
    char row = text[1];

    if (col < 'a' || col > 'h') return false;

    if (row < '1' || row > '8') return false;

    position_out->col = col - 'a';
    position_out->row = row - '1';

    return true;
}

static bool parse_expected_result(const char *expected_count_text, int *expected_count)
{
    //! complete
}

static bool validate_generated_moves(
    MoveList *moves,
    int expected_count,
    const char *expected_moves_text
)
{
    //! complete
}