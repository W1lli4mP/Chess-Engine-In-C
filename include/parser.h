#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "square.h"
#include "queue.h"
#include "san.h"

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

// err_pos to record the position of where the error occured if any
San *algebraic_chess_parser(const char *user_input, int *err_pos);

bool lexer(const char *user_input, int *err_pos, Queue *queue_out);

bool parser(Queue *token_queue, int *err_pos, San *san_out);

#endif