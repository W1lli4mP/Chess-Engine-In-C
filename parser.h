#ifndef PARSER_H
#define PARSER_H
#include "position.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// err_pos to record the position of where the error occured if any
bool algebraic_chess_parser(const char *user_input, int *err_pos, Position *position_out);

bool lexer(const char *user_input, int *err_pos, Queue *queue_out);

#endif