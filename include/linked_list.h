#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdbool.h>

typedef struct Node
{
    void *data;
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct
{
    Node *head;
    Node *tail;
} LinkedList;

Node *create_node(void);
void free_node(Node *node);

LinkedList *create_linked_list(void);
void free_linked_list(LinkedList *list);

bool append_linked_list(LinkedList *list, void *data);
void *pop_head_linked_list(LinkedList *list);
void *peek_head_linked_list(const LinkedList *list);

#endif