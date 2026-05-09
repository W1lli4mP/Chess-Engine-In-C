#include "linked_list.h"
#include <stdlib.h>

Node *create_node(void)
{
    Node *node = malloc(sizeof *node);
    if (!node) return NULL;

    node->data = NULL;
    node->next = NULL;
    node->prev = NULL;

    return node;
}

void free_node(Node *node)
{
    if (!node) return;
    free(node);
}

LinkedList *create_linked_list(void)
{
    LinkedList *list = malloc(sizeof *list);
    if (!list) return NULL;

    list->head = NULL;
    list->tail = NULL;

    return list;
}

void free_linked_list(LinkedList *list)
{
    if (!list) return;

    Node *curr = list->head;

    while (curr)
    {
        Node *next = curr->next;
        free_node(curr);
        curr = next;
    }

    free(list);
}

bool append_linked_list(LinkedList *list, void *data)
{
    if (!list) return false;

    Node *node = create_node();
    if (!node) return false;

    node->data = data;
    node->prev = list->tail;

    if (list->tail)
        list->tail->next = node;
    else
        list->head = node;

    list->tail = node;

    return true;
}

// now returns data pointer
void *pop_head_linked_list(LinkedList *list)
{
    if (!list || !list->head) return NULL;

    Node *old_head = list->head;
    void *data = old_head->data;

    list->head = old_head->next;

    if (list->head)
        list->head->prev = NULL;
    else
        list->tail = NULL;

    free_node(old_head);

    return data;
}

void *peek_head_linked_list(const LinkedList *list)
{
    if (!list || !list->head) return NULL;
    return list->head->data;
}