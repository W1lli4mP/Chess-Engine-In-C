#include <stdlib.h>
#include <string.h>
#include "queue.h"

Queue *create_queue(void)
{
    return create_linked_list();
}

bool push_queue(Queue *queue, const void *data, size_t data_size)
{
    if (!queue || !data || data_size == 0) return false;

    void *copy = malloc(data_size);
    if (!copy) return false;

    memcpy(copy, data, data_size);

    if (!append_linked_list(queue, copy))
    {
        free(copy);
        return false;
    }

    return true;
}

void *pop_queue(Queue *queue)
{
    return pop_head_linked_list(queue);
}

void *peek_queue(const Queue *queue)
{
    return peek_head_linked_list(queue);
}

void free_queue(Queue *queue)
{
    if (!queue) return;

    void *data;

    while ((data = pop_queue(queue)) != NULL)
    {
        free(data);
    }

    free_linked_list(queue);
}