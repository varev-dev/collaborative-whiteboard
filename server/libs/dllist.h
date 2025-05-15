#ifndef LLIST_H
#define LLIST_H

#include <stdlib.h>

typedef struct node_t {
    struct node_t   *prev;
    struct node_t   *next;
    void            *data;
} Node;

// Doubly Linked List
typedef struct dllist_t {
    Node    *head;
    Node    *tail;
    size_t  size;
} DLList;

DLList*  dllist_create();
void     dllist_destroy(DLList **list);
void     dllist_clear(DLList *list, void(*free_func)(void*));
void     dllist_push(DLList *list, void *data);
void*    dllist_pop(DLList *list);
void     dllist_foreach(DLList *list, void(*func)(void*));

// cmp_func must return 0 if elements match
void*    dllist_remove(DLList *list, void *data, int(*cmp_func)(void*, void*));
// cmp_func must return 0 if elements match
void*    dllist_find(DLList *list, void *data, int(*cmp_func)(void*, void*));

#endif // LLIST_H
