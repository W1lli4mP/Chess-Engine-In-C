#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stddef.h>

#include "linked_list.h"

typedef LinkedList Queue;

Queue *create_queue(void);
bool push_queue(Queue *queue, const void *data, size_t data_size);
void *pop_queue(Queue *queue);
void *peek_queue(const Queue *queue);
void free_queue(Queue *queue);

#endif