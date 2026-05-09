#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdio.h>
#include <stdlib.h>

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

Node *create_node();
void free_node(Node *node);
LinkedList *create_linked_list();
void free_linked_list(LinkedList *list);
void append_linked_list(LinkedList *list, void *data);
void remove_head_linked_list(LinkedList *list);
void print_char(void *ptr);
void print_int(void *ptr);
void print_string(void *ptr);

#endif