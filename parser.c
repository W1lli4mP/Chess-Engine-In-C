#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

int main(void) {
    // get input
    char user_input[20]; // safe input
    printf("Enter move: ");
    scanf("%19s", user_input);

    // tokenise
    int t = 0;
    char **tokens = tokenise(user_input, &t);

    printf("Tokens:\n");
    for (int i = 0; i < t; i++) {
        printf("%s\n", tokens[i]);
        free(tokens[i]); // free each token
    }
    free(tokens); // free array of pointers

    return 0;
}

char **tokenise(char *input_string, int *token_count) { // returns a list of strings
    char **tokens = malloc(10 * sizeof(char*)); // 7 is the max token limit but make it 10 to be sure
    int t = 0; // current no. of tokens

    int len = strlen(input_string);
    int i = 0;

    while (i < len) {
        char current_char = input_string[i];

        // CASTLE_O and CASTLE_OO tokens
        if (current_char == 'O' || current_char == '0') {
            // check for O-O/0-0
            if (i + 2 <= len) {
                char str_copy[6];
                strncpy(str_copy, input_string, 3); // syntax: destination, source, size_t
                str_copy[3] = '\0'; // manually add null terminator

                // strcmp() returns 0 if true
                if (strcmp(str_copy, "O-O") == 0 || strcmp(str_copy, "0-0") == 0) {
                    add_token_str(tokens, &t, str_copy);
                    i += 3;

                    // check for O-O-O/0-0-0
                    if (i + 2 <= len) {
                        strncpy(str_copy, input_string + i - 1, 5); // previous O-O + next char
                        str_copy[5] = '\0';
                        if (strcmp(str_copy, "O-O-O") == 0 || strcmp(str_copy, "0-0-0") == 0) {
                            add_token_str(tokens, &t, str_copy);
                            i += 2;
                        }
                    }
                    continue; // skip further processing
                }
            }
        }

        // PIECE token
        if (is_in(current_char, "KQRBN")) {
            add_token_char(tokens, &t, current_char);
            i++;
            continue;
        }

        // FILE token
        if (is_in(current_char, "abcdefgh")) {
            add_token_char(tokens, &t, current_char);
            i++;
            continue;
        }

        // ROW token
        if (is_in(current_char, "12345678")) {
            add_token_char(tokens, &t, current_char);
            i++;
            continue;
        }

        // CAPTURE token
        if (current_char == 'x') {
            add_token_char(tokens, &t, current_char);
            i++;
            continue;
        }

        // EQUALS token
        if (current_char == '=') {
            add_token_char(tokens, &t, current_char);
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