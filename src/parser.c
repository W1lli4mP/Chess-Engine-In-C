#include "parser.h"

// parser helpers
static bool expect(Queue *queue, Token token, Lex *lex_out, int *err_pos);
static bool accept(Queue *queue, Token token, Lex *lex_out);


static void parse_check(Queue *queue, San *san_out);
static bool parse_square(Queue *queue, Position *pos_out, int *err_pos);
static bool parse_promotion(Queue *queue, PieceType *piece_out, int *err_pos);
static bool parse_castle(Queue *queue, San *san_out, int *err_pos);
static bool parse_piece_move(Queue *queue, San *san_out, int *err_pos);

bool tokenisation(char c, Lex *l)
{
    switch (c)
    {
        case '+': case '#':
            l->token = TOKEN_CHECK;
            l->character = c;
            break;
        case '=':
            l->token = TOKEN_EQUAL;
            l->character = c;
            break;
        case 'x':
            l->token = TOKEN_CAPTURE;
            l->character = c;
            break;
        case '-':
            l->token = TOKEN_LINE;
            l->character = c;
            break;
        case 'O': case 'o': case '0':
            l->token = TOKEN_CIRCLE;
            l->character = c;
            break;
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h':
            l->token = TOKEN_COL;
            l->character = c;
            break;
        case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8':
            l->token = TOKEN_ROW;
            l->character = c;
            break;
        case 'R': case 'N': case 'B': case 'Q': case 'K':
            l->token = TOKEN_PIECE;
            l->character = c;
            break;
        // reject unknown chars
        return false;
    }
    return true;
}
static void char_to_pos(char col, char row, Position *pos)
{
    pos->col = col - 'a';
    pos->row = row - '1';
}

static PieceType char_to_piece_type(char piece)
{
    switch (piece)
    {
        case 'R': return TYPE_ROOK;
        case 'N': return TYPE_KNIGHT;
        case 'B': return TYPE_BISHOP;
        case 'Q': return TYPE_QUEEN;
        case 'K': return TYPE_KING;
        default: return TYPE_NONE;
    }
}

// helper to peek the nth node in a queue
static Lex *peek_n(Queue *queue, int n)
{
    Node *curr = queue->head;
    while (curr && n-- > 0) curr = curr->next;
    return (curr) ? (Lex *) curr->data : NULL;
}

// lexical analysis
bool lexer(const char *user_input, int *err_pos, Queue *queue_out)
{
    int len = strlen(user_input);

    for (int i = 0; i < len; i++)
    {
        Lex l;
        char c = user_input[i];

        // tokenise and store into lex
        if (!tokenisation(c, &l))
        {
            *err_pos = i;
            return false;
        }

        l.pos = i;
        push_queue(queue_out, &l, sizeof(Lex));
    }
    return true;
}

// syntax analysis
// token helpers for consuming
// strict - requires input token to match current token, else raises an error
static bool expect(Queue *queue, Token token, Lex *lex_out, int *err_pos)
{
    Lex *l = peek_queue(queue);
    if (!l || l->token != token)
    {
        if (l) *err_pos = l->pos;
        return false;
    }
    
    Lex *popped = pop_queue(queue);
    *lex_out = *popped;
    free(popped);
    return true;
}

// lenient - checks if input token matches current token, otherwise do nothing
static bool accept(Queue *queue, Token token, Lex *lex_out)
{
    Lex *l = peek_queue(queue);
    if (l && l->token == token)
    {
        Lex *popped = pop_queue(queue);
        *lex_out = *popped;
        free(popped);
        return true;
    }
    return false;
}

static void parse_check(Queue *queue, San *san_out)
{
    Lex temp;

    // + or # (optional for check/mate)
    if (accept(queue, TOKEN_CHECK, &temp))
    {
        if (temp.character == '+') san_out->suffix = SAN_SUFFIX_CHECK;
        if (temp.character == '#') san_out->suffix = SAN_SUFFIX_MATE;
    }
}

static bool parse_square(Queue *queue, Position *pos_out, int *err_pos)
{
    Lex temp;
    Lex *l = peek_queue(queue); // lookahead

    // column
    if (!accept(queue, TOKEN_COL, &temp))
    {
        if (l) *err_pos = l->pos;
        return false;
    }

    // store to update move if row is valid
    char col = temp.character;

    // row
    if (!expect(queue, TOKEN_ROW, &temp, err_pos)) return false;

    // store to update move
    char row = temp.character;

    // convert and populate pos_out
    char_to_pos(col, row, pos_out);
    return true;
}

static bool parse_promotion(Queue *queue, PieceType *piece_out, int *err_pos)
{
    Lex temp;

    // <equal>
    if (!accept(queue, TOKEN_EQUAL, &temp)) return false;

    // <piece>
    if (!expect(queue, TOKEN_PIECE, &temp, err_pos)) return false;

    *piece_out = char_to_piece_type(temp.character);

    //* reject pawn from being promoted to a king
    if (*piece_out == TYPE_KING)
    {
        *err_pos = temp.pos;
        return false;
    }

    return true;
}

static bool parse_castle(Queue *queue, San *san_out, int *err_pos)
{
    Lex temp;

    // O
    if (!expect(queue, TOKEN_CIRCLE, &temp, err_pos)) return false;

    // -
    if (!expect(queue, TOKEN_LINE, &temp, err_pos)) return false;

    // O
    if (!expect(queue, TOKEN_CIRCLE, &temp, err_pos)) return false;

    // - (optional for queenside)
    if (accept(queue, TOKEN_LINE, &temp))
    {
        // O
        if (!expect(queue, TOKEN_CIRCLE, &temp, err_pos)) return false;
        san_out->is_castle_queenside = true;
    }
    else
    {
        san_out->is_castle_kingside = true;
    }

    return true;
}

static bool parse_piece_move(Queue *queue, San *san_out, int *err_pos)
{
    Lex temp;

    // <piece>
    if (!expect(queue, TOKEN_PIECE, &temp, err_pos)) return false;

    san_out->piece = char_to_piece_type(temp.character);

    // lookahead
    Lex *t1 = peek_queue(queue); // next token
    Lex *t2 = peek_n(queue, 1); // next next token

    // <disambiguation>?
    // column disambiguation
    if (t1 && t1->token == TOKEN_COL)
    {
        if (!(t2 && t2->token == TOKEN_ROW))
        {
            accept(queue, TOKEN_COL, &temp);
            san_out->from_col = temp.character - 'a';
        }
    }
    // rank disambiguation
    else if (t1 && t1->token == TOKEN_ROW)
    {
        if (!(t2 && t2->token == TOKEN_COL))
        {
            accept(queue, TOKEN_ROW, &temp);
            san_out->from_row = temp.character - '1';
        }
    }

    // <capture>?
    if (accept(queue, TOKEN_CAPTURE, &temp))
    {
        san_out->is_capture = true;
    }

    // <square>
    Position to;
    if (!parse_square(queue, &to, err_pos)) return false;

    san_out->to = to;

    return true;
}

static bool parse_pawn_move(Queue *queue, San *san_out, int *err_pos)
{
    // lookahead initialisation and validation
    Lex *a = peek_queue(queue);
    if (!a) return false;

    // <col>
    if (a->token != TOKEN_COL)
    {
        *err_pos = a->pos;
        return false;
    }

    Lex *b = peek_n(queue, 1);
    if (!b)
    {
        *err_pos = a->pos;
        return false;
    }

    san_out->piece = TYPE_PAWN;

    // <row>?
    if (b->token == TOKEN_ROW)
    {
        // <square>
        Position to;
        if (!parse_square(queue, &to, err_pos)) return false;
        san_out->to = to;

        // <promotion>?
        Lex *promotion_start = peek_queue(queue);
        if (promotion_start && promotion_start->token == TOKEN_EQUAL)
        {
            PieceType promotion;
            if (!parse_promotion(queue, &promotion, err_pos)) return false;
            san_out->is_promotion = true;
            san_out->promotion = promotion;
        }

        return true;
    }

    // <capture>?
    if (b->token == TOKEN_CAPTURE)
    {
        Lex temp;

        // <col>
        if (!expect(queue, TOKEN_COL, &temp, err_pos)) return false;
        san_out->from_col = temp.character - 'a';

        // <capture>
        if (!expect(queue, TOKEN_CAPTURE, &temp, err_pos)) return false;
        san_out->is_capture = true;

        // <square>
        Position to;
        if (!parse_square(queue, &to, err_pos)) return false;
        san_out->to = to;

        // <promotion>?
        Lex *promotion_start = peek_queue(queue);
        if (promotion_start && promotion_start->token == TOKEN_EQUAL)
        {
            PieceType promotion;
            if (!parse_promotion(queue, &promotion, err_pos)) return false;
            san_out->is_promotion = true;
            san_out->promotion = promotion;
        }

        return true;
    }
    
    *err_pos = b->pos;
    return false;
}

bool parser(Queue *token_queue, int *err_pos, San *san_out)
{
    if (!token_queue || !token_queue->head) return false;

    // <move> ::= <castle> | <piece-move> | <pawn-move>
    Lex *first = peek_queue(token_queue);
    if (!first) return false;

    // <castle>
    if (first->token == TOKEN_CIRCLE)
    {
        if (!parse_castle(token_queue, san_out, err_pos)) return false;
    }

    // <piece-move>
    else if (first->token == TOKEN_PIECE)
    {
        if (!parse_piece_move(token_queue, san_out, err_pos)) return false;
    }

    // <pawn-move>
    else
    {
        if (!parse_pawn_move(token_queue, san_out, err_pos)) return false;
    }

    // parse for checks on trailing tokens
    parse_check(token_queue, san_out);

    // check if there are still trailing tokens
    Lex *leftover = peek_queue(token_queue);
    if (leftover)
    {
        *err_pos = leftover->pos;
        return false;
    }

    return true;
}


// convert algebraic chess notation -> playable move
San *algebraic_chess_parser(const char *user_input, int *err_pos)
{
    Queue *token_queue = initialise_queue();

    San *san = initialise_san();
    
    // tokenise and store into a queue
    if (!lexer(user_input, err_pos, token_queue))
    {
        free_queue(token_queue);
        destroy_san(san);
        return NULL;
    }

    // parse and consume tokens then populate a move struct
    if (!parser(token_queue, err_pos, san))
    {
        free_queue(token_queue);
        destroy_san(san);
        return NULL;
    }

    // garbage collection
    free_queue(token_queue);

    return san;
}