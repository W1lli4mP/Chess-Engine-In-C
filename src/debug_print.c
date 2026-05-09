#include "debug_print.h"

static char piece_type_to_char(PieceType p)
{
    switch (p)
    {
        case TYPE_ROOK: return 'R';
        case TYPE_KNIGHT: return 'N';
        case TYPE_BISHOP: return 'B';
        case TYPE_QUEEN: return 'Q';
        case TYPE_KING: return 'K';
        case TYPE_PAWN: return 'P';
        default: return '-';
    }
}

void debug_print_piece(const Piece *piece)
{
    if (!piece)
    {
        putchar(' ');
        return;
    }

    printf("%s", piece->sprite);
}

void debug_print_board(const Board *board, int white_pov)
{
    if (!board) return;

    // setup variables for the loop based on the board POV
    int row_start, row_end, row_step;
    int col_start = 0;
    int col_end = board->width;
    int col_step = 1;

    if (white_pov)
    {
        row_start = board->height - 1;
        row_end = -1;
        row_step = -1;
    }
    else
    {
        row_start = 0;
        row_end = board->height;
        row_step = 1;
    }

    for (int row = row_start; row != row_end; row += row_step)
    {
        for (int col = col_start; col != col_end; col += col_step)
        {
            Piece *piece = board->grid[row][col];

            if (!piece)
                printf(" ");
            else
                debug_print_piece(piece);
            
            printf(" ");
        }

        putchar('\n');
    }

}

void debug_print_move(Move move)
{
    printf(
        "[MOVE] %c%c%c -> %c%c\n",
        piece_type_to_char(move.piece),
        move.from.col + 'a',
        move.from.row + '1',
        move.to.col + 'a',
        move.to.row + '1'
    );
}

void debug_print_san(San san)
{
    if (san.is_castle_kingside)
    {
        puts("[SAN] Attempting to castle kingside");
        return;
    }

    if (san.is_castle_queenside)
    {
        puts("[SAN] Attempting to castle queenside");
        return;
    }

    if (san.from_col != -1)
    {
        printf(
            "[SAN] Attempting to move %c from %c-file to %c%c\n",
            piece_type_to_char(san.piece),
            san.from_col + 'a',
            san.to.col + 'a',
            san.to.row + '1'
        );
    }
    else
    {
        printf(
            "[SAN] Attempting to move %c to %c%c\n",
            piece_type_to_char(san.piece),
            san.to.col + 'a',
            san.to.row + '1'
        );
    }
}

void debug_print_resolve_status(ResolveStatus status)
{

    switch (status)
    {
        case RESOLVE_OK:
            puts("[SAN RESOLVER] LEGAL MOVE!");
            break;

        case RESOLVE_AMBIGUOUS:
            puts("[SAN RESOLVER] AMBIGUOUS MOVE!");
            break;

        case RESOLVE_ILLEGAL:
            puts("[SAN RESOLVER] ILLEGAL MOVE!");
            break;

        default:
            puts("[SAN RESOLVER] UNKNOWN STATUS!");
            break;
    }
}

#ifdef ENGINE_DEBUG

static void debug_move_to_text(Move move, char out[8])
{
    out[0] = (char)('a' + move.from.col);
    out[1] = (char)('1' + move.from.row);
    out[2] = (char)('a' + move.to.col);
    out[3] = (char)('1' + move.to.row);
    out[4] = '\0';

    if (move.is_promotion)
    {
        char promotion = '?';

        switch (move.promotion)
        {
            case TYPE_QUEEN:  promotion = 'q'; break;
            case TYPE_ROOK:   promotion = 'r'; break;
            case TYPE_BISHOP: promotion = 'b'; break;
            case TYPE_KNIGHT: promotion = 'n'; break;
            default: break;
        }

        out[4] = promotion;
        out[5] = '\0';
    }
}

void debug_print_engine_move_score(Move move, int score)
{
    char text[8];
    debug_move_to_text(move, text);

    printf("[ENGINE] %s score %d\n", text, score);
}

void debug_print_engine_best_move(Move move, int score)
{
    char text[8];
    debug_move_to_text(move, text);

    printf("[ENGINE] best %s score %d\n", text, score);
}

#endif