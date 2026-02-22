#include "queue.h"

Queue *initialise_queue()
{
    return initialise_linked_list();
}

void push_queue(Queue *queue, void *data, size_t data_size)
{
    void *copy = malloc(data_size);
    memcpy(copy, data, data_size);
    append_linked_list(queue, copy);
}

void *pop_queue(Queue *queue)
{
    void *data;
    if (!queue->head) return NULL;
    data = queue->head->data;
    remove_head_linked_list(queue);
    
    return data;
}

void *peek_queue(Queue *queue)
{
    if (!queue->head) return NULL;
    return queue->head->data;
}

void free_queue(Queue *queue)
{
    void *data;
    while ((data = pop_queue(queue)) != NULL) free(data);
    free_linked_list(queue);
}