#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include "test_utils.h"
#include "board.h"
#include "game_state.h"
#include "move_apply.h"
#include "fen_parser.h"
#include "move_gen.h"

#define PERFT_CASES_FILE "tests/data/perft_cases.txt"
#define MAX_LINE_LEN 256

static void print_perft_result(
    const char *test_id,
    bool passed,
    const char *reason
);

static bool run_perft_case(
    const char *test_id,
    const char *fen,
    const char *depth,
    const char *expected_nodes
);

static bool parse_depth(const char *text, int *depth_out);

static bool parse_expected_nodes(const char *text, uint64_t *nodes_out);

static uint64_t perft(GameState *game, int depth, bool *ok_out);

static void print_perft_divide(GameState *game, int depth);

static void move_to_text(Move move, char out[16]);

static char promotion_piece_to_char(PieceType type);

int main()
{
    puts("------------------------");

    FILE *fp = fopen(PERFT_CASES_FILE, "r");
    if (!fp)
    {
        perror("Failed to open Perft test file");
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

        // keep the original line for debug outputs of malformed lines
        char original[MAX_LINE_LEN];
        strncpy(original, line, sizeof original);
        original[sizeof original - 1] = '\0';

        char *fields[4];
        const char *split_error = NULL;

        if (!split_test_line(line, fields, 4, &split_error))
        {
            printf("Malformed test line (%s): %s\n", split_error, original);
            continue;
        }
        
        // extract fields
        char *test_id = fields[0];
        char *fen = fields[1];
        char *depth = fields[2];
        char *expected_nodes = fields[3];

        // process test case
        total++;

        if (run_perft_case(test_id, fen, depth, expected_nodes)) passed++;
    }

    fclose(fp);

    printf("Passed %d/%d\n", passed, total);

    return passed != total;
}

static void print_perft_result(
    const char *test_id,
    bool passed,
    const char *reason
)
{
    printf("Test: %s\n", test_id);

    if (passed)
        puts("Result: PASS");
    else
        printf("Result: FAIL (reason = %s)\n", reason);
    
    puts("------------------------");
}

static bool run_perft_case(
    const char *test_id,
    const char *fen,
    const char *depth_text,
    const char *expected_nodes_text
)
{
    // parse depth
    int depth = 0;
    if (!parse_depth(depth_text, &depth))
    {
        print_perft_result(test_id, false, "invalid depth");
        return false;
    }

    // parse expected nodes
    uint64_t expected_nodes = 0;
    if (!parse_expected_nodes(expected_nodes_text, &expected_nodes))
    {
        print_perft_result(test_id, false, "invalid expected nodes");
        return false;
    }

    // actual perft testing logic
    GameState *game = create_game_state();

    if (!game)
    {
        print_perft_result(test_id, false, "failed to create game state");
        return false;
    }

    int err_pos = -1;

    // load FEN
    if (!load_fen(game, fen, &err_pos))
    {
        destroy_game_state(game);

        char reason[128];
        snprintf(reason, sizeof reason, "failed to load FEN at err_pos = %d", err_pos);
        print_perft_result(test_id, false, reason);

        return false;
    }

    bool ok = true;
    uint64_t actual_nodes = perft(game, depth, &ok);

    if (!ok)
    {
        destroy_game_state(game);
        print_perft_result(test_id, false, "perft failed internally");
        return false;
    }

    if (actual_nodes != expected_nodes)
    {
        printf("Expected nodes: %" PRIu64 "\n", expected_nodes);
        printf("Actual nodes: %" PRIu64 "\n", actual_nodes);

        if (depth > 0)
        {
            print_perft_divide(game, depth);
        }

        destroy_game_state(game);
        print_perft_result(test_id, false, "node count mismatch");
        return false;
    }

    destroy_game_state(game);
    print_perft_result(test_id, true, "PASS");
    return true;
}

static bool parse_depth(const char *text, int *depth_out)
{
    if (!text || !depth_out) return false;

    char *end = NULL;
    long value = strtol(text, &end, 10);

    // no digits parsed
    if (end == text) return false;
    
    // trailing chars
    if (*end != '\0') return false;
    
    // depth cannot be negative
    if (value < 0) return false;

    *depth_out = (int) value;
    return true;
}

static bool parse_expected_nodes(const char *text, uint64_t *nodes_out)
{
    if (!text || !nodes_out) return false;

    char *end = NULL;
    unsigned long long value = strtoull(text, &end, 10);

    if (end == text) return false;
    if (*end != '\0') return false;

    *nodes_out = (uint64_t) value;
    return true;
}

static uint64_t perft(GameState *game, int depth, bool *ok_out)
{
    if (!game || !ok_out)
    {
        if (ok_out) *ok_out = false;
        return 0;
    }

    // base case
    if (depth == 0) return 1;

    MoveList moves = {0};

    if (!generate_all_legal_moves(game, &moves))
    {
        *ok_out = false;
        return 0;
    }

    uint64_t nodes = 0;

    // simulate all moves
    for (int i = 0; i < moves.count; i++)
    {
        Move move = moves.moves[i];
        UndoInfo undo;

        // play the move
        if (!make_move(game, move, &undo))
        {
            *ok_out = false;
            return 0;
        }

        // update count of nodes
        nodes += perft(game, depth - 1, ok_out);

        // restore move to prevent corrupted game states
        if (!*ok_out)
        {
            unmake_move(game, move, &undo);
            return 0;
        }

        // restore the move
        if (!unmake_move(game, move, &undo))
        {
            *ok_out = false;
            return 0;
        }
    }

    return nodes;
}

static void print_perft_divide(GameState *game, int depth)
{
    if (!game || depth <= 0) return;

    MoveList moves = {0};

    if (!generate_all_legal_moves(game, &moves))
    {
        puts("Failed to generate divide moves");
        return;
    }

    uint64_t total = 0;

    puts("Perft divide:");

    for (int i = 0; i < moves.count; i++)
    {
        Move move = moves.moves[i];
        UndoInfo undo;

        if (!make_move(game, move, &undo))
        {
            puts("make_move failed during divide");
            return;
        }

        bool ok = true;
        uint64_t nodes = perft(game, depth - 1, &ok);

        if (!unmake_move(game, move, &undo))
        {
            puts("unmake_move failed during divide");
            return;
        }

        if (!ok)
        {
            puts("perft failed during divide");
            return;
        }

        char move_text[16];
        move_to_text(move, move_text);

        printf("%s: %" PRIu64 "\n", move_text, nodes);

        total += nodes;
    }

    printf("Divide total: %" PRIu64 "\n", total);
}

static void move_to_text(Move move, char out[16])
{
    out[0] = (char)('a' + move.from.col);
    out[1] = (char)('1' + move.from.row);
    out[2] = (char)('a' + move.to.col);
    out[3] = (char)('1' + move.to.row);
    out[4] = '\0';

    if (move.is_promotion)
    {
        out[4] = promotion_piece_to_char(move.promotion);
        out[5] = '\0';
    }
}

static char promotion_piece_to_char(PieceType type)
{
    switch (type)
    {
        case TYPE_QUEEN: return 'q';
        case TYPE_ROOK: return 'r';
        case TYPE_BISHOP: return 'b';
        case TYPE_KNIGHT: return 'n';
        default: return '?';
    }
}