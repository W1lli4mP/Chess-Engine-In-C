#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include <string.h>

#include "linked_list.h"

typedef LinkedList Queue;

Queue *create_queue();
void push_queue(Queue *queue, void *data, size_t data_size);
void *pop_queue(Queue *queue);
void *peek_queue(Queue *queue);
void free_queue(Queue *queue);

#endif