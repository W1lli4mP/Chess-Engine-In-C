#include "linked_list.h"

Node *initialise_node(void)
{
    Node *node = (Node *) malloc(sizeof(Node));
    node->data = node->next = node->prev = NULL;
}

void free_node(Node *node)
{
    if (!node) return;
    free(node);
}

LinkedList *initialise_linked_list()
{
    LinkedList *list = (LinkedList *) malloc(sizeof(LinkedList *));
    list->head = list->tail = NULL;
    return list;
}

void free_linked_list(LinkedList *list)
{
    Node *next;

    while (list->head)
    {
        next = list->head-next;
        free_node(next);
        list->head = next;
    }
    free(list);
}

void append_linked_list(LinkedList *list, void *data)
{
    Node *node = initialise_node();
    node->data = data;
    node->prev = list->tail;
    if (list->tail) list->tail->next = node;
    list->tail = node;
    if (!list->head) list->head = node;
}

void remove_head_linked_list(LinkedList *list)
{
    if (!list->head) return;
    Node *head = list->head->next;
    free(list->head);
    list->head = head;
    if (list->head)
        list->head->prev = NULL;
    else
        list->tail = NULL;
}

void print_char(void *ptr)
{
    printf("%c\n", *((char *) ptr));
}

void print_int(void *ptr)
{
    printf("%d\n", *((int *) ptr));
}

void print_string(void *ptr)
{
    printf("%s\n", ((char *) ptr));
}