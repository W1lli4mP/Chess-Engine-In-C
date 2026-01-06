#include "parser.h"

typedef enum
{
    TOKEN_CHECK,
    TOKEN_EQUAL,
    TOKEN_CAPTURE,
    TOKEN_LINE,
    TOKEN_CIRCLE,
    TOKEN_COL,
    TOKEN_ROW,
    TOKEN_PIECE
} Token;

typedef struct
{
    Token token;
    char character;
    int pos;
} Lex;

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

// convert Position (row, col) -> algebraic chess notation
bool algebraic_chess_parser(const char *user_input, int *err_pos, Position *position_out)
{
    Queue *token_queue = initialise_queue();
    
    // tokenise and store into a queue
    if (!lexer(user_input, err_pos, token_queue))
    {
        return false;
    }

    // parse and consume tokens then populate a move struct

}

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

void parser()
{

}