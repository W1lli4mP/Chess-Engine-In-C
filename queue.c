#include "queue.h"

Queue *initialise_queue()
{
    return initialise_linked_list();
}

void push_queue(Queue *queue, void *data, size_t data_size)
{
    append_linked_list(queue, NULL);
    memcpy(queue->tail->data, data, data_size);
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
    while (data = pop_queue(queue)) free(queue);
    free_linked_list(queue);
}