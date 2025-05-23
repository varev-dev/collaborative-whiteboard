#include "dllist.h"

#include <assert.h>

DLList* dllist_create() {
    DLList *l = malloc(sizeof(DLList));
    if (l == NULL) {
        return NULL;
    }

    l->tail = l->head = NULL;
    l->size = 0;

    return l;
}

void dllist_free(DLList **list) {
    assert(list != NULL && *list != NULL);

    Node *prev, *curr = (*list)->head;

    while (curr != NULL) {
        prev = curr;
        curr = curr->next;
        free(prev);
    }

    free(*list);
    *list = NULL;
}

void dllist_push(DLList *l, void *data) {
    assert(l != NULL);

    Node *node = malloc(sizeof(Node));
    node->data = data;
    node->next = NULL;

    if (l->head == NULL) {
        l->head = node;
        node->prev = NULL;
    } else if (l->tail == NULL) {
        l->tail = node;
        node->prev = l->head;
        l->head->next = l->tail;
    } else {
        l->tail->next = node;
        node->prev = l->tail;
        l->tail = node;
    }

    l->size++;
}

void dllist_pushfront(DLList *l, void *data) {
    assert(l != NULL);

    Node *front = malloc(sizeof(Node));
    if (front == NULL) {
        return;
    }

    front->data = data;
    front->prev = NULL;
    front->next = l->head;
    l->head->prev = front;
    l->head = front;
    l->size++;
}

void* dllist_remove(DLList *l, void *data, int(*cmp_func)(void*, void*)) {
    assert(l != NULL && cmp_func != NULL);

    if (l->size == 0) {
        return NULL;
    }

    Node *curr = l->head;

    while (curr != NULL && cmp_func(curr->data, data) != 0) {
        curr = curr->next;
    }

    if (curr == NULL) {
        return NULL;
    }

    void *d = curr->data;
    if (curr->prev != NULL) {
        curr->prev->next = curr->next;
    } else {
        l->head = curr->next;
    }

    if (curr->next != NULL) {
        curr->next->prev = curr->prev;
    } else {
        l->tail = curr->prev;
    }

    free(curr);
    l->size--;

    return d;
}


void* dllist_pop(DLList *l) {
    assert(l != NULL);

    if (l->size == 0) {
        return NULL;
    }

    void *data;
    l->size--;

    if (l->tail == NULL) {
        data = l->head->data;
        free(l->head);
        l->head = NULL;
        return data;
    }

    Node *tail = l->tail;
    data = tail->data;
    tail->prev->next = NULL;

    if (l->size == 1) {
        l->tail = NULL;
    } else {
        l->tail = tail->prev;
    }

    free(tail);

    return data;
}

void* dllist_popfront(DLList *l) {
    assert(l != NULL);

    if (l->size == 0) {
        return NULL;
    }

    Node* front = l->head;
    void* data = l->head->data;

    l->head = l->head->next;
    if (l->head != NULL) {
        l->head->prev = NULL;

        if (l->head == l->tail) {
            l->tail = NULL;
        }
    }

    l->size--;
    free(front);

    return data;
}

void* dllist_find(DLList *l, void *data, int(*cmp_func)(void*, void*)) {
    assert(l != NULL && cmp_func != NULL);

    if (l->size == 0) {
        return NULL;
    }

    Node* curr = l->head;

    while (curr != NULL && cmp_func(curr->data, data) != 0) {
        curr = curr->next;
    }

    if (curr == NULL) {
        return NULL;
    }

    return curr->data;
}

void dllist_foreach(DLList *l, void(*func)(void*, void*), void *arg2) {
    assert(l != NULL && func != NULL);

    if (l->size == 0) {
        return;
    }

    Node *curr = l->head;

    while (curr != NULL) {
        func(curr->data, arg2);
        curr = curr->next;
    }
}

void dllist_clear(DLList *l, void(*free_func)(void*, void*), void *arg2) {
    assert(l != NULL && free_func != NULL);

    if (l->size == 0) {
        return;
    }

    Node *prev, *curr = l->head;

    while (curr != NULL) {
        prev = curr;
        curr = curr->next;

        free_func(prev->data, arg2);
        free(prev);
    }

    l->head = NULL;
    l->tail = NULL;
}
