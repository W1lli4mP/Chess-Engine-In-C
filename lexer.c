#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

// MOVED TO PARSER.C
// int main(void) {
//     // get input
//     char user_input[20]; // safe input
//     printf("Enter move: ");
//     scanf("%19s", user_input);

//     // tokenise
//     int t = 0;
//     Token **tokens = tokenise(user_input, &t);

//     printf("Tokens:\n");
//     for (int i = 0; i < t; i++) {
//         printf("%s\n", tokens[i]->value);
//         free(tokens[i]->value); // free each token
//     }
//     free(tokens); // free array of pointers

//     return 0;
// }

Token **tokenise(char *input_string, int *token_count) { // returns a list of strings
    Token **tokens = malloc(10 * sizeof(Token*)); // 7 is the max token limit but make it 10 to be sure
    int t = 0; // current no. of tokens

    int len = strlen(input_string);
    int i = 0;

    while (i < len) {
        char current_char = input_string[i];

        // CASTLE_O and CASTLE_OO tokens
        if (current_char == 'O' || current_char == '0') {
            // check for O-O-O/0-0-0
            if (i + 5 <= len) {
                char str_copy[6];
                strncpy(str_copy, input_string + i, 5); // previous O-O + next char
                str_copy[5] = '\0';

                if (strcmp(str_copy, "O-O-O") == 0 || strcmp(str_copy, "0-0-0") == 0) {
                    Token token = init_token(CASTLE_OO, str_copy);
                    add_token(tokens, &t, token);
                    i += 5;
                    continue;
                }
            }
        }

            // check for O-O/0-0
            if (i + 3 <= len) {
                char str_copy[6];
                strncpy(str_copy, input_string + i, 3); // syntax: destination, source, size_t
                str_copy[3] = '\0'; // manually add null terminator

                // strcmp() returns 0 if true
                if (strcmp(str_copy, "O-O") == 0 || strcmp(str_copy, "0-0") == 0) {
                    Token token = init_token(CASTLE_O, str_copy);
                    add_token(tokens, &t, token);
                    i += 3;
                    continue;
                }
            }

        // PIECE token
        if (is_in(current_char, "KQRBN")) {
            char v[2] = {current_char, '\0'};
            Token token = init_token(PIECE, v);
            add_token(tokens, &t, token);
            i++;
            continue;
        }

        // FILE token
        if (is_in(current_char, "abcdefgh")) {
            char v[2] = {current_char, '\0'};
            Token token = init_token(C_FILE, v);
            add_token(tokens, &t, token);
            i++;
            continue;
        }

        // RANK token
        if (is_in(current_char, "12345678")) {
            char v[2] = {current_char, '\0'};
            Token token = init_token(C_RANK, v);
            add_token(tokens, &t, token);
            i++;
            continue;
        }

        // CAPTURES token
        if (current_char == 'x') {
            char v[2] = {current_char, '\0'};
            Token token = init_token(CAPTURES, v);
            add_token(tokens, &t, token);
            i++;
            continue;
        }

        // EQUALS token
        if (current_char == '=') {
            char v[2] = {current_char, '\0'};
            Token token = init_token(EQUALS, v);
            add_token(tokens, &t, token);
            i++;
            continue;
        }
        i++; // skips over unknown chars
    }

    *token_count = t;
    return tokens;

}

int is_in(char x, char *string) {
    return strchr(string, x) ? 1 : 0;
}

// add a single char token
void add_token_char(char **tokens, int *t, char current_char) { // tokens pointer, t_index pointer, char to add
    // tokens is an array of strings, thus cannot add a single char
    tokens[*t] = malloc(2); // allocates 2 bits of memory, 1 char + 1 null terminator
    tokens[*t][0] = current_char;
    tokens[*t][1] = '\0';
    (*t)++;
}

// add a str token
void add_token_str(char **tokens, int *t, char *current_string) {
    tokens[*t] = malloc(strlen(current_string) + 1);
    strcpy(tokens[*t], current_string);
    (*t)++;
}

void add_token(Token **tokens, int *t, Token token) {
    tokens[*t] = malloc(sizeof(Token));
    tokens[*t]->type = token.type;
    tokens[*t]->value = token.value;
    (*t)++;
}

Token init_token(TokenType type, char *value) {
    Token t;
    t.type = type;
    if (value) {
        t.value = malloc(strlen(value) + 1);
        if (!t.value) {perror("malloc"); exit(1);} // prints error msg if value not defined
        strcpy(t.value, value);
    } else {
        t.value = NULL;
    }
    return t;
}
