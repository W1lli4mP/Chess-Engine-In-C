#include "fen_parser.h"

static bool fen_char_to_piece(char c, PieceType *type_out, Colour *colour_out);

static bool parse_uint(const char *fen, int *i, int *int_out, int *err_pos);
static bool expect_space(const char *fen, int *i, int *err_pos);

static bool parse_piece_placement(Board *board, const char *fen, int *i, int *err_pos);
static bool parse_side_to_move(const char *fen, int *i, FenMeta *meta_out, int *err_pos);
static bool parse_castling(Board *board, const char *fen, int *i, int *err_pos);
static bool parse_en_passant(const char *fen, int *i, FenMeta *meta_out, int *err_pos);

// side helpers
static bool fen_char_to_piece(char c, PieceType *type_out, Colour *colour_out)
{
    switch (c)
    {
        case 'P': *type_out = TYPE_PAWN;   *colour_out = COLOUR_WHITE; return true;
        case 'R': *type_out = TYPE_ROOK;   *colour_out = COLOUR_WHITE; return true;
        case 'N': *type_out = TYPE_KNIGHT; *colour_out = COLOUR_WHITE; return true;
        case 'B': *type_out = TYPE_BISHOP; *colour_out = COLOUR_WHITE; return true;
        case 'Q': *type_out = TYPE_QUEEN;  *colour_out = COLOUR_WHITE; return true;
        case 'K': *type_out = TYPE_KING;   *colour_out = COLOUR_WHITE; return true;

        case 'p': *type_out = TYPE_PAWN;   *colour_out = COLOUR_BLACK; return true;
        case 'r': *type_out = TYPE_ROOK;   *colour_out = COLOUR_BLACK; return true;
        case 'n': *type_out = TYPE_KNIGHT; *colour_out = COLOUR_BLACK; return true;
        case 'b': *type_out = TYPE_BISHOP; *colour_out = COLOUR_BLACK; return true;
        case 'q': *type_out = TYPE_QUEEN;  *colour_out = COLOUR_BLACK; return true;
        case 'k': *type_out = TYPE_KING;   *colour_out = COLOUR_BLACK; return true;

        default: return false;
    }
}

// parses a decimal integer
static bool parse_uint(const char *fen, int *i, int *int_out, int *err_pos)
{
    if (!isdigit((unsigned char) fen[*i]))
    {
        if (err_pos) *err_pos = *i;
        return false;
    }

    int value = 0;
    while (isdigit((unsigned char) fen[*i]))
    {
        value = value * 10 + (fen[*i] - '0');
        (*i)++;
    }

    *int_out = value;
    return true;
}

static bool expect_space(const char *fen, int *i, int *err_pos)
{
    if (fen[*i] != ' ')
    {
        if (err_pos) *err_pos = *i;
        return false;
    }

    (*i)++;
    return true;
}

// main helpers
// parses the first FEN field
static bool parse_piece_placement(Board *board, const char *fen, int *i, int *err_pos)
{
    for (int fen_row = 0; fen_row < 8; fen_row++)
    {
        int col = 0;
        int board_row = 7 - fen_row;

        // read chars until '/' or ' ' reached
        while (fen[*i] && fen[*i] != '/' && fen[*i] != ' ')
        {
            char c = fen[*i];

            // digits represent a series of empty squares
            if (c >= '1' && c <= '8')
            {
                col += (c - '0');

                if (col > 8)
                {
                    if (err_pos) *err_pos = *i;
                    return false;
                }
                (*i)++;
            }
            else
            {
                PieceType type;
                Colour colour;

                // validate and decode piece character
                if (!fen_char_to_piece(c, &type, &colour))
                {
                    if (err_pos) *err_pos = *i;
                    return false;
                }
                
                if (col >= 8)
                {
                    if (err_pos) *err_pos = *i;
                    return false;
                }

                // create piece and add to board
                Piece *piece = create_piece(type, colour);
                Position piece_position = { .col = col, .row = board_row };
                if (!piece)
                {
                    if (err_pos) *err_pos = *i;
                    return false;
                }

                if (!set_piece_at(board, piece_position, piece))
                {
                    destroy_piece(piece);
                    if (err_pos) *err_pos = *i;
                    return false;
                }

                col++;
                (*i)++;
            }
        }

        // must have exactly 8 squares
        if (col != 8)
        {
            if (err_pos) *err_pos = *i;
            return false;
        }
        
        if (fen_row < 7)
        {
            if (fen[*i] != '/')
            {
                if (err_pos) *err_pos = *i;
                return false;
            }
            (*i)++;
        }
    }

    return true;
}

static bool parse_side_to_move(const char *fen, int *i, FenMeta *meta_out, int *err_pos)
{
    if (fen[*i] == 'w')
    {
        if (meta_out) meta_out->white_to_move = true;
        (*i)++;
        return true;
    }

    if (fen[*i] == 'b')
    {
        if (meta_out) meta_out->white_to_move = false;
        (*i)++;
        return true;
    }

    if (err_pos) *err_pos = *i;
    return false;
}

static bool parse_castling(Board *board, const char *fen, int *i, int *err_pos)
{
    // reset all castling rights before reading the field
    board->white_can_castle_kingside = false;
    board->white_can_castle_queenside = false;
    board->black_can_castle_kingside = false;
    board->black_can_castle_queenside = false;

    if (fen[*i] == '-')
    {
        (*i)++;
        return true;
    }

    bool seen_any = false;
    
    // consume castling rights
    while (fen[*i] && fen[*i] != ' ')
    {
        switch (fen[*i])
        {
            case 'K':
                if (board->white_can_castle_kingside)
                {
                    if (err_pos) *err_pos = *i;
                    return false;
                }
                board->white_can_castle_kingside = true;
                break;
            
            case 'Q':
                if (board->white_can_castle_queenside)
                {
                    if (err_pos) *err_pos = *i;
                    return false;
                }
                board->white_can_castle_queenside = true;
                break;
            
            case 'k':
                if (board->black_can_castle_kingside)
                {
                    if (err_pos) *err_pos = *i;
                    return false;
                }
                board->black_can_castle_kingside = true;
                break;

            case 'q':
                if (board->black_can_castle_queenside)
                {
                    if (err_pos) *err_pos = *i;
                    return false;
                }
                board->black_can_castle_queenside = true;
                break;

            // unexpected characters render the entire field as invalid
            default:
                if (err_pos) *err_pos = *i;
                return false;
        }

        seen_any = true;
        (*i)++;
    }

    // if field was not '-' then at least one valid symbol must have been read
    if (!seen_any)
    {
        if (err_pos) *err_pos = *i;
        return false;
    }

    return true;
}

static bool parse_en_passant(const char *fen, int *i, FenMeta *meta_out, int *err_pos)
{
    // default to no en passant target
    if (meta_out) meta_out->has_en_passant = false;

    // check if there is no en passant square
    if (fen[*i] == '-')
    {
        (*i)++;
        return true;
    }

    char col = fen[*i];
    char row = fen[*i + 1];

    // validate col and row
    if (col < 'a' || col > 'h')
    {
        if (err_pos) *err_pos = *i;
        return false;
    }

    if (row < '1' || row > '8')
    {
        if (err_pos) *err_pos = *i + 1;
        return false;
    }

    // store parsed squaare in metadata
    if (meta_out)
    {
        meta_out->has_en_passant = true;
        meta_out->en_passant.col = col - 'a';
        meta_out->en_passant.row = row - '1';
    }

    // consume both characters of the square
    *i += 2;
    return true;
}

// parses a complete FEN string and loads it into the board
// parsing is done on a temporary board to separate conflicts
// actual board is modified iff entire FEN is valid
bool load_fen(Board *board_out, const char *fen, FenMeta *meta_out, int *err_pos)
{
    if (!board_out || !fen)
    {
        if (err_pos) *err_pos = 0;
        return false;
    }

    // make temporary empty board instead of overwriting the actual board straight away
    Board *temp = initialise_empty_board();

    if (!temp)
    {
        if (err_pos) *err_pos = 0;
        return false;
    }

    // initialise metadata defaults before parsing
    if (meta_out)
    {
        meta_out->white_to_move = true;
        meta_out->has_en_passant = false;
        meta_out->en_passant.row = -1;
        meta_out->en_passant.col = -1;
        meta_out->halfmove_clock = 0;
        meta_out->fullmove_number = 1;
    }

    // current index for parsing (helps tracks errors)
    int i = 0;

    //* FEN structure:
    //* <piece-placement> <side-to-move> <castling> <en-passant> <halfmove> <fullmove>

    // first four fields
    if (!parse_piece_placement(temp, fen, &i, err_pos)) goto fail;
    if (!expect_space(fen, &i, err_pos)) goto fail;

    if (!parse_side_to_move(fen, &i, meta_out, err_pos)) goto fail;
    if (!expect_space(fen, &i, err_pos)) goto fail;

    if (!parse_castling(temp, fen, &i, err_pos)) goto fail;
    if (!expect_space(fen, &i, err_pos)) goto fail;

    if (!parse_en_passant(fen, &i, meta_out, err_pos)) goto fail;
    if (!expect_space(fen, &i, err_pos)) goto fail;

    // halfmove clock field
    if (meta_out)
    {
        if (!parse_uint(fen, &i, &meta_out->halfmove_clock, err_pos)) goto fail;
    }
    else
    {
        int ignored;
        if (!parse_uint(fen, &i, &ignored, err_pos)) goto fail;
    }

    if (!expect_space(fen, &i, err_pos)) goto fail;

    // fullmove number field
    if (meta_out)
    {
        if (!parse_uint(fen, &i, &meta_out->fullmove_number, err_pos)) goto fail;
        if (meta_out->fullmove_number < 1)
        {
            if (err_pos) *err_pos = i;
            goto fail;
        }   
    }
    else
    {
        int ignored;
        if (!parse_uint(fen, &i, &ignored, err_pos)) goto fail;
        if (ignored < 1)
        {
            if (err_pos) *err_pos = i;
            goto fail;
        }
    }

    // reject any trailing characters after 6 required FEN fields
    if (fen[i] != '\0')
    {
        if (err_pos) *err_pos = i;
        goto fail;
    }

    // overwrite actual board
    if (!clear_board(board_out))
    {
        if (err_pos) *err_pos = 0;
        goto fail;
    }

    // transfer piece ownership from temp board to original board
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            board_out->grid[row][col] = temp->grid[row][col];
            temp->grid[row][col] = NULL;
        }
    }

    // copy all attributes
    board_out->height = temp->height;
    board_out->width = temp->width;

    board_out->white_can_castle_kingside = temp->white_can_castle_kingside;
    board_out->white_can_castle_queenside = temp->white_can_castle_queenside;
    board_out->black_can_castle_kingside = temp->black_can_castle_kingside;
    board_out->black_can_castle_queenside = temp->black_can_castle_queenside;

    destroy_board(temp);
    return true;

fail:
    // cleanup temporary board and partially created pieces on failure
    destroy_board(temp);
    return false;
}