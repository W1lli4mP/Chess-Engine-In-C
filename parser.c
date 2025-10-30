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
void parse_program(void);
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

    printf("Enter move: ");
    scanf("%19s", user_input);

    // tokenise input
    t = 0;
    tokens = tokenise(user_input, tp);
    printf("Tokens: ");
    for (int i = 0; i < t; i++) {
        printf("%s ", tokens[i]->value);
        // free(tokens[i]->value); // free each token
    }
    printf("\n");

    // free(tokens); // free array of pointers
    
    // parse_move();
    // cur++;
    parse_program();

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

void parse_program(void) {
    while (cur < t) {
        parse_move();
        cur++;
    }
}