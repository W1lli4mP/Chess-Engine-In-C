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

typedef struct
{
    Position to;
    bool is_promotion;
    PieceType promotion;
} ExpectedMove;

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

static bool parse_expected_count(const char *text, int *count_out);

static bool validate_generated_moves(
    const MoveList *moves,
    int expected_count,
    const char *expected_moves_text
);

static bool parse_expected_move(const char *text, ExpectedMove *expected_out);

static bool expected_move_seen(
    const ExpectedMove *seen,
    int seen_count,
    ExpectedMove expected
);

static bool move_list_contains_expected_move(
    const MoveList *moves,
    ExpectedMove expected
);

static bool parse_promotion_piece(char c, PieceType *promotion_out);

static void print_expected_moves(const char *expected_moves_text);

static void print_move_list_expected_format(const MoveList *moves);

static void position_to_square(Position position, char out[3]);

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

        // keep the original line for debug outputs of malformed lines
        char original[MAX_LINE_LEN] ;
        strncpy(original, line, sizeof original);
        original[sizeof original - 1] = '\0';

        // 1st separator
        char *sep1 = strchr(line, '|');

        if (!sep1)
        {
            printf("Malformed test line: %s\n", original);
            continue;
        }

        *sep1 = '\0';

        char *fen = sep1 + 1;

        // 2nd separator
        char *sep2 = strchr(fen, '|');

        if (!sep2)
        {
            printf("Malformed test line: %s\n", original);
            continue;
        }

        *sep2 = '\0';

        char *from = sep2 + 1;

        // 3rd separator
        char *sep3 = strchr(from, '|');

        if (!sep3)
        {
            printf("Malformed test line: %s\n", original);
            continue;
        }

        *sep3 = '\0';

        char *expected_count = sep3 + 1;

        // 4th separator
        char *sep4 = strchr(expected_count, '|');

        if (!sep4)
        {
            printf("Malformed test line: %s\n", original);
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
    // parse square
    Position from;

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

        // add FEN parser's err position indexing
        char reason[128];
        snprintf(reason, sizeof reason, "failed to load FEN at err_pos = %d", err_pos);
        print_move_gen_result(test_id, false, reason);

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

    // report failed test cases
    if (!ok)
    {
        printf("Expected count: %d\n", expected_count);
        printf("Actual count: %d\n", moves.count);

        printf("Expected moves: ");
        print_expected_moves(expected_moves_text);

        printf("Actual moves: ");
        print_move_list_expected_format(&moves);

        puts("------------------------");
    }

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
    
    // print a separator only when passed; other function prints a separator when failed
    if (passed)
    {
        puts("Result: PASS");
        puts("------------------------");
    }
    else
        printf("Result: FAIL (reason = %s)\n", reason);
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

static bool parse_expected_count(const char *text, int *count_out)
{
    if (!text || !count_out) return false;

    char *end = NULL;
    long value = strtol(text, &end, 10);

    // no digits were parsed
    if (end == text) return false;

    // check for extra characters after the number
    if (*end != '\0') return false;

    // move counts cannot be negative
    if (value < 0) return false;

    //* cannot exceed max moves
    if (value > MAX_MOVES) return false;

    *count_out = (int) value;
    return true;
}

static bool validate_generated_moves(
    const MoveList *moves,
    int expected_count,
    const char *expected_moves_text
)
{
    if (!moves || !expected_moves_text) return false;

    if (moves->count != expected_count) return false;

    // if no moves are expected, expected moves field should be empty
    if (expected_count == 0) return expected_moves_text[0] == '\0';

    // create a copy and store into a buffer
    char buffer[MAX_LINE_LEN];
    strncpy(buffer, expected_moves_text, sizeof buffer);
    buffer[sizeof buffer - 1] = '\0';

    ExpectedMove seen[MAX_MOVES];
    int seen_count = 0;

    // tokenise each expected move/square
    char *token = strtok(buffer, ",");

    while (token)
    {
        ExpectedMove expected;

        // <square> | <square> <equals> <promotion>
        if (!parse_expected_move(token, &expected)) return false;

        // cannot have repeated moves in the test case
        if (expected_move_seen(seen, seen_count, expected)) return false;

        // if there is ONE move not in the generated move list, it fails
        if (!move_list_contains_expected_move(moves, expected)) return false;

        // guard so count does not exceed maximum moves allocated
        if (seen_count >= MAX_MOVES) return false;
        seen[seen_count++] = expected;

        token = strtok(NULL, ",");
    }

    return seen_count == expected_count;
}

static bool parse_expected_move(const char *text, ExpectedMove *expected_out)
{
    if (!text || !expected_out) return false;

    // verify text length
    size_t len = strlen(text);

    if (len != 2 && len != 4) return false;

    // extract square from text then parse
    char square_text[3] = { text[0], text[1], '\0' };

    if (!parse_square(square_text, &expected_out->to)) return false;

    expected_out->is_promotion = false;
    expected_out->promotion = TYPE_NONE;

    if (len == 4)
    {
        if (text[2] != '=') return false;

        expected_out->is_promotion = true;

        if (!parse_promotion_piece(text[3], &expected_out->promotion)) return false;
    }

    return true;
}

static bool expected_move_seen(
    const ExpectedMove *seen,
    int seen_count,
    ExpectedMove expected
)
{
    if (!seen) return false;

    // skip already observed moves
    for (int i = 0; i < seen_count; i++)
    {
        if (
            seen[i].to.row == expected.to.row &&
            seen[i].to.col == expected.to.col &&
            seen[i].is_promotion == expected.is_promotion &&
            seen[i].promotion == expected.promotion
        ) return true;
    }

    return false;
}

static bool move_list_contains_expected_move(
    const MoveList *moves,
    ExpectedMove expected
)
{
    if (!moves) return false;

    for (int i = 0; i < moves->count; i++)
    {
        Move move = moves->moves[i];

        // skip mismatches
        if (move.to.row != expected.to.row || move.to.col != expected.to.col) continue;
        
        if (move.is_promotion != expected.is_promotion) continue;

        if (move.is_promotion && move.promotion != expected.promotion) continue;

        return true;
    }

    return false;
}

static bool parse_promotion_piece(char c, PieceType *promotion_out)
{
    if (!promotion_out) return false;

    switch (c)
    {
        case 'Q': *promotion_out = TYPE_QUEEN; return true;
        case 'R': *promotion_out = TYPE_ROOK; return true;
        case 'B': *promotion_out = TYPE_BISHOP; return true;
        case 'N': *promotion_out = TYPE_KNIGHT; return true;
        default: return false;
    }
}

static void print_expected_moves(const char *expected_moves_text)
{
    if (!expected_moves_text || expected_moves_text[0] == '\0')
        puts("(none)");
    else
        puts(expected_moves_text);
}

// renamed due to printing more than just destinations now
static void print_move_list_expected_format(const MoveList *moves)
{
    if (!moves)
    {
        puts("(null)");
        return;
    }

    for (int i = 0; i < moves->count; i++)
    {
        char square[3];
        position_to_square(moves->moves[i].to, square);

        printf("%s", square);

        // check for promotion
        if (moves->moves[i].is_promotion)
        {
            char promotion = '?';

            // set promotion piece type
            switch (moves->moves[i].promotion)
            {
                case TYPE_QUEEN: promotion = 'Q'; break;
                case TYPE_ROOK: promotion = 'R'; break;
                case TYPE_BISHOP: promotion = 'B'; break;
                case TYPE_KNIGHT: promotion = 'N'; break;
                default: break;
            }

            printf("=%c", promotion);
        }

        // add commas if there is a subsequent move
        if (i + 1 < moves->count) putchar(',');   
    }

    putchar('\n');
}

static void position_to_square(Position position, char out[3])
{
    out[0] = (char) ('a' + position.col);
    out[1] = (char) ('1' + position.row);
    out[2] = '\0';
}