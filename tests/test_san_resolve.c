#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "game_state.h"
#include "fen_parser.h"
#include "san_resolve.h"
#include "position.h"
#include "parser.h"
#include "test_utils.h"

#define SAN_RESOLVE_CASES_FILE "tests/data/san_resolve_cases.txt"
#define MAX_LINE_LEN 256

typedef struct
{
    bool capture;
    bool promotion;
    PieceType promotion_piece;
    bool castle_kingside;
    bool castle_queenside;
} ExpectedFlags;

static void print_san_resolve_result(
    const char *test_id,
    bool passed,
    const char *reason
);

static bool run_san_resolve_case(
    const char *test_id,
    const char *fen,
    const char *san,
    const char *expected_status,
    const char *expected_from,
    const char *expected_to,
    const char *expected_flags
);

static bool parse_status(const char *text, ResolveStatus *status_out);

static bool parse_square(const char *text, Position *position_out);

static bool parse_promotion_piece(char c, PieceType *promotion_out);

static bool parse_flags(const char *text, ExpectedFlags *flags_out);

int main()
{
    puts("------------------------");

    FILE *fp = fopen(SAN_RESOLVE_CASES_FILE, "r");
    if (!fp)
    {
        perror("Failed to open San Resolve test file");
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
        char original[MAX_LINE_LEN] ;
        strncpy(original, line, sizeof original);
        original[sizeof original - 1] = '\0';

        char *fields[7];
        const char *split_error = NULL;

        // partition fields
        if (!split_test_line(line, fields, 7, &split_error))
        {
            printf("Malformed test line (%s): %s\n", split_error, original);
            continue;
        }

        // extract fields
        char *test_id = fields[0];
        char *fen = fields[1];
        char *san = fields[2];
        char *expected_status = fields[3];
        char *expected_from = fields[4];
        char *expected_to = fields[5];
        char *expected_flags = fields[6];

        // process test case
        total++;

        if (
            run_san_resolve_case(
                test_id,
                fen,
                san,
                expected_status,
                expected_from,
                expected_to,
                expected_flags
            )
        ) passed++;
    }

    fclose(fp);

    printf("Passed %d/%d tests\n", passed, total);
    
    return passed != total;
}

//* MAIN HELPER
static void print_san_resolve_result(
    const char *test_id,
    bool passed,
    const char *reason
)
{
    printf("Test: %s\n", test_id);
    
    if (passed)
    {
        puts("Result: PASS");
    }
    else
        printf("Result: FAIL (reason = %s)\n", reason);
}

static bool run_san_resolve_case(
    const char *test_id,
    const char *fen,
    const char *san_text,
    const char *expected_status_text,
    const char *expected_from_text,
    const char *expected_to_text,
    const char *expected_flags_text
)
{
    // parse expected status
    ResolveStatus expected_status;

    if (!parse_status(expected_status_text, &expected_status))
    {
        print_san_resolve_result(test_id, false, "invalid expected status");
        return false;
    }

    // parse squares IFF status is OK
    Position expected_from = { .row = -1, .col = -1 };
    Position expected_to = { .row = -1, .col = -1 };

    if (expected_status == RESOLVE_OK)
    {
        // parse expected from
        if (!parse_square(expected_from_text, &expected_from))
        {
            print_san_resolve_result(test_id, false, "invalid expected from square");
            return false;
        }

        // parse expected to
        if (!parse_square(expected_to_text, &expected_to))
        {
            print_san_resolve_result(test_id, false, "invalid expected to square");
            return false;
        }
    }

    // parse expected flags
    ExpectedFlags expected_flags;
    if (!parse_flags(expected_flags_text, &expected_flags))
    {
        print_san_resolve_result(test_id, false, "invalid expected flags");
        return false;
    }

    // setup game state
    GameState *game = create_game_state();

    if (!game)
    {
        print_san_resolve_result(test_id, false, "failed to create game state");
        return false;
    }

    // setup empty board state
    game->board = initialise_empty_board();

    if (!game->board)
    {
        destroy_game_state(game);
        print_san_resolve_result(test_id, false, "generated moves did not match expected moves");
        return false;
    }

    // load FEN
    int fen_err_pos = -1;
    
    if (!load_fen(game, fen, &fen_err_pos))
    {
        destroy_game_state(game);

        char reason[128];
        snprintf(reason, sizeof reason, "failed to load FEN at err_pos = %d", fen_err_pos);
        print_san_resolve_result(test_id, false, reason);

        return false;
    }

    // parse SAN
    int san_err_pos = -1;

    San *san = algebraic_chess_parser(san_text, &san_err_pos);

    if (!san)
    {
        destroy_san(san);
        destroy_game_state(game);
        
        char reason[128];
        snprintf(reason, sizeof reason, "failed to parse SAN at err_pos = %d", san_err_pos);

        return false;
    }

    // resolve SAN
    Move move;
    ResolveStatus actual_status = resolve_san(game, *san, &move);

    // fail mismatched resolve status'
    if (actual_status != expected_status)
    {
        destroy_san(san);
        destroy_game_state(game);
        print_san_resolve_result(test_id, false, "resolved status did not match expected status");
        return false;
    }

    // if the status is not OK, test immediately pass
    //? this is because the status aligns with the expected status, meaning no further validation is required
    if (expected_status != RESOLVE_OK)
    {
        destroy_san(san);
        destroy_game_state(game);
        print_san_resolve_result(test_id, true, "PASS");
        return true;
    }

    // validate OK moves
    // from and to validation
    if (move.from.row != expected_from.row || move.from.col != expected_from.col)
    {
        destroy_san(san);
        destroy_game_state(game);
        print_san_resolve_result(test_id, false, "resolved from square did not match");
        return false;
    }

    if (move.to.row != expected_to.row || move.to.col != expected_to.col)
    {
        destroy_san(san);
        destroy_game_state(game);
        print_san_resolve_result(test_id, false, "resolved to square did not match");
        return false;
    }

    // flag validation
    if (move.is_capture != expected_flags.capture)
    {
        destroy_san(san);
        destroy_game_state(game);
        print_san_resolve_result(test_id, false, "resolved capture flag did not match");
        return false;
    }

    if (move.is_promotion != expected_flags.promotion)
    {
        destroy_san(san);
        destroy_game_state(game);
        print_san_resolve_result(test_id, false, "resolved promotion flag did not match");
        return false;
    }

    if (move.is_promotion && move.promotion != expected_flags.promotion_piece)
    {
        destroy_san(san);
        destroy_game_state(game);
        print_san_resolve_result(test_id, false, "resolved promotion piece did not match");
        return false;
    }

    if (move.is_castle_kingside != expected_flags.castle_kingside)
    {
        destroy_san(san);
        destroy_game_state(game);
        print_san_resolve_result(test_id, false, "resolved kingside castle flag did not match");
        return false;
    }

    if (move.is_castle_queenside != expected_flags.castle_queenside)
    {
        destroy_san(san);
        destroy_game_state(game);
        print_san_resolve_result(test_id, false, "resolved queenside castle flag did not match");
        return false;
    }

    destroy_san(san);
    destroy_game_state(game);
    print_san_resolve_result(test_id, true, "PASS");
    return true;
}

// HELPERS
static bool parse_status(const char *text, ResolveStatus *status_out)
{
    if (!text || !status_out) return false;

    // parse OK
    if (strcmp(text, "OK") == 0)
    {
        *status_out = RESOLVE_OK;
        return true;
    }

    // parse ILLEGAL
    if (strcmp(text, "ILLEGAL") == 0)
    {
        *status_out = RESOLVE_ILLEGAL;
        return true;
    }

    // parse AMBIGUOUS
    if (strcmp(text, "AMBIGUOUS") == 0)
    {
        *status_out = RESOLVE_AMBIGUOUS;
        return true;
    }

    return false;
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

static bool parse_flags(const char *text, ExpectedFlags *flags_out)
{
    if (!text || !flags_out) return false;

    // set all default attributes before modifying
    *flags_out = (ExpectedFlags) {0};
    flags_out->promotion_piece = TYPE_NONE;

    if (strcmp(text, "none") == 0 || text[0] == '\0') return true;

    // copy field before tokenising
    char buffer[MAX_LINE_LEN];
    strncpy(buffer, text, sizeof buffer);
    buffer[sizeof buffer - 1] = '\0';

    // tokenise
    char *token = strtok(buffer, ",");

    // consume all tokens and update expected flags accordingly
    while (token)
    {
        if (strcmp(token, "capture") == 0)
        {
            flags_out->capture = true;
        }
        else if (strcmp(token, "castle_kingside") == 0)
        {
            flags_out->castle_kingside = true;
        }
        else if (strcmp(token, "castle_queenside") == 0)
        {
            flags_out->castle_queenside = true;
        }
        else if(strcmp(token, "promotion=") == 0)
        {
            flags_out->promotion = true;

            // reject mismatched length of "promotion=X"
            if (strlen(token) != 11) return false;

            // parse promotion piece
            if (!parse_promotion_piece(token[10], &flags_out->promotion_piece)) return false;
        }
        else
        {
            return false;
        }
        
        // consume token and move onto the next
        token = strtok(NULL, ",");
    }

    return true;
}