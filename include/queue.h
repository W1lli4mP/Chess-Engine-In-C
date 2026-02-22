#ifndef QUEUE_H
#define QUEUE_H
#include "linked_list.h"
#include <stdlib.h>
#include <string.h>

typedef LinkedList Queue;

Queue *initialise_queue();
void push_queue(Queue *queue, void *data, size_t data_size);
void *pop_queue(Queue *queue);
void *peek_queue(Queue *queue);
void free_queue(Queue *queue);

#endif