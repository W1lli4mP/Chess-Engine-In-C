#ifndef PARSER_H
#define PARSER_H

typedef struct {
    char *type; // allows the tokeniser to differentiate between the types of tokens - making it a lexer
    char *value; // contains the token info
} Token;

char **tokenise(char*, int*);
int is_in(char, char*);
void add_token_char(char**, int*, char);
void add_token_str(char**, int*, char*);

#endif