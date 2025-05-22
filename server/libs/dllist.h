#ifndef DLLIST_H
#define DLLIST_H

#include <stdlib.h>

typedef struct node {
    struct node *prev;
    struct node *next;
    void        *data;
} Node;

// Doubly Linked List
typedef struct dllist {
    Node   *head;
    Node   *tail;
    size_t size;
} DLList;

DLList*  dllist_create();
void     dllist_destroy(DLList **list);
void     dllist_clear(DLList *list, void(*free_func)(void*, void*), void *arg2);
void     dllist_push(DLList *list, void *data);
void     dllist_pushfront(DLList *list, void *data);
void*    dllist_pop(DLList *list);
void*    dllist_popfront(DLList *list);
void     dllist_foreach(DLList *list, void(*func)(void*, void*), void *arg2);

// cmp_func must return 0 if elements match
void*   dllist_remove(DLList *list, void *data, int(*cmp_func)(void*, void*));
// cmp_func must return 0 if elements match
void*   dllist_find(DLList *list, void *data, int(*cmp_func)(void*, void*));

#endif // DLLIST_H
