#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lexer.h"

// helper functions
Token *peek(void);
Token *accept(TokenType);
Token expect(TokenType);
bool at_end(void);

// parse functions
void parse_program(Token**);
void parse_move(void);

// global pointers
int t;
int *tp = &t;

Token **tokens;
Token ***tkp = &tokens;

int cur = 0; // current pointer to token
int *curp = &cur;

int main(void) {
    // get input
    char user_input[20]; // safe input

    // lexer
    printf("Enter move: ");
    scanf("%19s", user_input);
    tokens = tokenise(user_input, tp);

    int t = 0;
    print_tokens(tokens, t);
    
    // parser
    parse_program(tokens);

    free(tokens);

    return 0;
}

// returns token at current index
Token *peek(void) { // returns token/NULL
    return (at_end()) ? NULL : tokens[cur];
}

Token *accept(TokenType type) { // returns token/NULL
    if (peek() != NULL) {
        if (peek()->type == type) {
            // add consume logic later
            return peek();
        }
    }
    return NULL;
}

bool at_end(void) {
    return (tokens == NULL) || (cur >= *tp);
}


void parse_move(void) {
    (accept(PIECE) != NULL) ? printf("passed\n") : printf("failed\n");
}

void parse_program(Token** tokens) {
    while (cur < t) {
        parse_move();
        cur++;
    }
}