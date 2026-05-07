#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "game_state.h"
#include "fen_parser.h"
#include "move_apply.h"

#define MOVE_APPLY_CASES_FILE "tests/data/move_apply_cases.txt"
#define MAX_LINE_LEN 256

static bool run_move_apply_case(
    const char *test_id,
    const char *fen,
    const char *from_text,
    const char *move_text,
    const char *expected_after_fen_like_note_text
);

static void print_move_apply_result(
    const char *test_id,
    bool passed,
    const char *reason
);

static bool parse_square(const char *text, Position *position_out);

static bool positions_equal(Position a, Position b);

static bool verify_after_make(
    const GameState *game,
    Move move,
    Piece *moved_piece,
    Piece *captured_piece,
    Colour original_side_to_move,
    int original_fullmove_number
);

static bool verify_after_unmake(
    const GameState *game,
    Move move,
    Piece *moved_piece,
    Piece *captured_piece,
    Colour original_side_to_move,
    int original_halfmove_clock,
    int original_fullmove_number,
    CastlingRights original_castling_rights,
    bool original_has_en_passant_target,
    Position original_en_passant_target
);

int main()
{
    puts("------------------------");

    FILE *fp = fopen(MOVE_APPLY_CASES_FILE, "r");
    if (!fp)
    {
        perror("Failed to open Move Apply test file");
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

        char *to = sep3 + 1;

        // 4th separator
        char *sep4 = strchr(to, '|');

        if (!sep4)
        {
            printf("Malformed test line: %s\n", original);
            continue;
        }

        *sep4 = '\0';

        char *expected_after = sep4 + 1;
        
        // process test case
        total++;

        if (run_move_apply_case(test_id, fen, from, to, expected_after)) passed++;
    }

    fclose(fp);

    printf("Passed %d/%d tests\n", passed, total);

    return passed != total;
}

//* MAIN HELPER
static bool run_move_apply_case(
    const char *test_id,
    const char *fen,
    const char *from_text,
    const char *to_text,
    const char *expected_after_text
)
{
    // parse from square
    Position from;

    if (!parse_square(from_text, &from))
    {
        print_move_apply_result(test_id, false, "invalid from square");
        return false;
    }

    // parse to square
    Position to;

    if (!parse_square(to_text, &to))
    {
        print_move_apply_result(test_id, false, "invalid to square");
        return false;
    }

    // setup game state
    GameState *game = create_game_state();

    if (!game)
    {
        print_move_apply_result(test_id, false, "failed to create game state");
        return false;
    }

    // setup empty board state
    game->board = initialise_empty_board();

    if (!game->board)
    {
        destroy_game_state(game);
        print_move_apply_result(test_id, false, "failed to create board");
        return false;
    }

    int err_pos = -1;

    // load FEN
    if (!load_fen(game, fen, &err_pos))
    {
        destroy_game_state(game);

        // add FEN parser's err position indexing
        char reason[128];
        snprintf(reason, sizeof reason, "failed to load FEN at err pos = %d", err_pos);
        print_move_apply_result(test_id, false, reason);

        return false;
    }

    //* verify game states pre and post make_move and unmake_move
    //! reserved for future en passant/castling/promotion expectations
    (void) expected_after_text; // TEMPORARY

    Piece *moved_piece = get_piece_at(game->board, from);

    if (!moved_piece)
    {
        destroy_game_state(game);
        print_move_apply_result(test_id, false, "no piece on from square");
        return false;
    }

    // store original game state
    Piece *captured_piece = get_piece_at(game->board, to);

    Colour original_side_to_move = game->side_to_move;
    CastlingRights original_castling_rights = game->castling_rights;
    bool original_has_en_passant_target = game->has_en_passant_target;
    Position original_en_passant_target = game->en_passant_target;
    int original_halfmove_clock = game->halfmove_clock;
    int original_fullmove_number = game->fullmove_number;

    // create move
    Move move = create_move(moved_piece->type, from, to);

    // update move with captured flag
    if (captured_piece) move.is_capture = true;

    // update move with promotion flags if expected after text specifies it
    if (strcmp(expected_after_text, "promotion=Q") == 0)
    {
        move.is_promotion = true;
        move.promotion = TYPE_QUEEN;
    }
    else if (strcmp(expected_after_text, "promotion=R") == 0)
    {
        move.is_promotion = true;
        move.promotion = TYPE_ROOK;
    }
    else if (strcmp(expected_after_text, "promotion=B") == 0)
    {
        move.is_promotion = true;
        move.promotion = TYPE_BISHOP;
    }
    else if (strcmp(expected_after_text, "promotion=N") == 0)
    {
        move.is_promotion = true;
        move.promotion = TYPE_KNIGHT;
    }

    // populate UndoInfo object to unmake the move afterwards
    UndoInfo undo;

    // simulate the move and the verify correct game states
    if (!make_move(game, move, &undo))
    {
        destroy_game_state(game);
        print_move_apply_result(test_id, false, "make_move failed");
        return false;
    }

    if (!verify_after_make(
        game,
        move,
        moved_piece,
        captured_piece,
        original_side_to_move,
        original_fullmove_number
    ))
    {
        destroy_game_state(game);
        print_move_apply_result(test_id, false, "state after make_move is incorrect");
        return false;
    }

    // verify all of UndoInfo's attributes before unmaking the move
    if (undo.captured_piece != captured_piece)
    {
        destroy_game_state(game);
        print_move_apply_result(test_id, false, "undo captured piece is incorrect");
        return false;
    }

    if (!positions_equal(undo.captured_position, to))
    {
        destroy_game_state(game);
        print_move_apply_result(test_id, false, "undo captured position is incorrect");
        return false;
    }

    if (undo.previous_side_to_move != original_side_to_move)
    {
        destroy_game_state(game);
        print_move_apply_result(test_id, false, "undo side to move is incorrect");
        return false;
    }

    if (undo.previous_halfmove_clock != original_halfmove_clock)
    {
        destroy_game_state(game);
        print_move_apply_result(test_id, false, "undo halfmove clock is incorrect");
        return false;
    }

    if (undo.previous_fullmove_number != original_fullmove_number)
    {
        destroy_game_state(game);
        print_move_apply_result(test_id, false, "undo fullmove number is incorrect");
        return false;
    }

    // restore the move and verify correct game states
    if (!unmake_move(game, move, &undo))
    {
        destroy_game_state(game);
        print_move_apply_result(test_id, false, "unmake_move failed");
        return false;
    }

    if (!verify_after_unmake(
        game,
        move,
        moved_piece,
        captured_piece,
        original_side_to_move,
        original_halfmove_clock,
        original_fullmove_number,
        original_castling_rights,
        original_has_en_passant_target,
        original_en_passant_target
    ))
    {
        destroy_game_state(game);
        print_move_apply_result(test_id, false, "state after unmake_move is incorrect");
        return false;
    }

    // pass after complete verification of making and unmaking the move
    destroy_game_state(game);
    print_move_apply_result(test_id, true, "PASS");
    return true;
}

static void print_move_apply_result(
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

static bool positions_equal(Position a, Position b)
{
    return a.col == b.col && a.row == b.row;
}

static bool verify_after_make(
    const GameState *game,
    Move move,
    Piece *moved_piece,
    Piece *captured_piece,
    Colour original_side_to_move,
    int original_fullmove_number
)
{
    if (!game || !game->board || !moved_piece) return false;

    // verify pieces
    Piece *from_piece = get_piece_at(game->board, move.from);
    Piece *to_piece = get_piece_at(game->board, move.to);

    // square should be empty where the piece had just moved from
    if (from_piece != NULL) return false;

    // destination square should contain the piece that has just moved
    if (to_piece != moved_piece) return false;

    // verify promotion
    if (move.is_promotion && to_piece->type != move.promotion) return false;

    //* NORMAL GAME STATE ATTRIBUTES

    // verify side
    Colour expected_side = (original_side_to_move == COLOUR_WHITE) ? COLOUR_BLACK : COLOUR_WHITE;

    // current side to move should be inverted with the next side to move
    if (game->side_to_move != expected_side) return false;

    // verify fullmove number
    int expected_fullmove = original_fullmove_number;

    // increment to match game's future fullmove number
    if (expected_side == COLOUR_WHITE) expected_fullmove++;

    if (game->fullmove_number != expected_fullmove) return false;

    // captured piece should be captured and not stay in its original square
    if (captured_piece && to_piece == captured_piece) return false;

    return true;
}

static bool verify_after_unmake(
    const GameState *game,
    Move move,
    Piece *moved_piece,
    Piece *captured_piece,
    Colour original_side_to_move,
    int original_halfmove_clock,
    int original_fullmove_number,
    CastlingRights original_castling_rights,
    bool original_has_en_passant_target,
    Position original_en_passant_target
)
{
    if (!game || !game->board || !moved_piece) return false;

    //* verify all attributes of current game state to the previous game state
    Piece *from_piece = get_piece_at(game->board, move.from);
    Piece *to_piece = get_piece_at(game->board, move.to);

    //* NORMAL GAME STATE ATTRIBUTES
    if (from_piece != moved_piece) return false;

    // promotion (check AFTER verifying moved piece)
    if (from_piece->type != move.piece) return false;

    if (to_piece != captured_piece) return false;

    if (game->side_to_move != original_side_to_move) return false;

    if (game->halfmove_clock != original_halfmove_clock) return false;

    if (game->fullmove_number != original_fullmove_number) return false;

    if (game->castling_rights.white_can_castle_kingside != original_castling_rights.white_can_castle_kingside) return false;
    if (game->castling_rights.white_can_castle_queenside != original_castling_rights.white_can_castle_queenside) return false;
    if (game->castling_rights.black_can_castle_kingside != original_castling_rights.black_can_castle_kingside) return false;
    if (game->castling_rights.black_can_castle_queenside != original_castling_rights.black_can_castle_queenside) return false;

    if (game->has_en_passant_target != original_has_en_passant_target) return false;

    if (!positions_equal(game->en_passant_target, original_en_passant_target)) return false;

    return true;
}