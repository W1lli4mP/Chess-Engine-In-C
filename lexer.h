#ifndef LEXER_H
#define LEXER_H

typedef enum {
    PIECE,
    C_FILE, // file is already a reserved keyword in stdlib
    C_RANK,
    CAPTURE,
    EQUALS,
    CASTLE_O,
    CASTLE_OO,
    UNKNOWN
} TokenType;

typedef struct {
    TokenType type; // allows the tokeniser to differentiate between the types of tokens - making it a lexer
    char *value; // contains the token info
} Token;

Token **tokenise(char*, int*);
int is_in(char, char*);
void add_token_char(char**, int*, char);
void add_token_str(char**, int*, char*);
void add_token(Token**, int*, Token);
Token init_token(TokenType, char*);

#endif