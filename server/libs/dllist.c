#include "dllist.h"

DLList* dllist_create() {
    DLList *list = malloc(sizeof(DLList));
    if (list == NULL) {
        return NULL;
    }

    list->tail = list->head = NULL;
    list->size = 0;

    return list;
}

void dllist_destroy(DLList **list) {
    if (list == NULL || *list == NULL) {
        return;
    }

    Node *prev, *curr = (*list)->head;

    while (curr != NULL) {
        prev = curr;
        curr = curr->next;
        free(prev);
    }

    free(*list);
    *list = NULL;
}

void dllist_push(DLList *list, void *data) {
    if (list == NULL) {
        return;
    }

    Node *node = malloc(sizeof(Node));
    node->data = data;
    node->next = NULL;

    if (list->head == NULL) {
        list->head = node;
        node->prev = NULL;
    } else if (list->tail == NULL) {
        list->tail = node;
        node->prev = list->head;
        list->head->next = list->tail;
    } else {
        list->tail->next = node;
        node->prev = list->tail;
        list->tail = node;
    }

    list->size++;
}

void dllist_pushfront(DLList *list, void *data) {
    if (list == NULL) {
        return;
    }

    Node *front = malloc(sizeof(Node));
    if (front == NULL) {
        return;
    }

    front->data = data;
    front->prev = NULL;
    front->next = list->head;
    list->head->prev = front;
    list->head = front;
}

void* dllist_remove(DLList *list, void *data, int(*cmp_func)(void*, void*)) {
    if (list == NULL || list->size == 0 || cmp_func == NULL) {
        return NULL;
    }

    Node *curr = list->head;

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
        list->head = curr->next;
    }

    if (curr->next != NULL) {
        curr->next->prev = curr->prev;
    } else {
        list->tail = curr->prev;
    }

    free(curr);
    list->size--;

    return d;
}


void* dllist_pop(DLList *list) {
    if (list == NULL || list->size == 0) {
        return NULL;
    }

    void *data;
    list->size--;

    if (list->tail == NULL) {
        data = list->head->data;
        free(list->head);
        list->head = NULL;
        return data;
    }

    Node *tail = list->tail;
    data = tail->data;
    tail->prev->next = NULL;

    if (list->size == 1) {
        list->tail = NULL;
    } else {
        list->tail = tail->prev;
    }

    free(tail);

    return data;
}

void* dllist_popfront(DLList *list) {
    if (list == NULL || list->size == 0) {
        return NULL;
    }

    Node* front = list->head;
    void* data = list->head->data;

    list->head = list->head->next;
    if (list->head != NULL) {
        list->head->prev = NULL;

        if (list->head == list->tail) {
            list->tail = NULL;
        }
    }

    free(front);

    return data;
}

void* dllist_find(DLList *list, void *data, int(*cmp_func)(void*, void*)) {
    if (list == NULL || list->size == 0 || cmp_func == NULL) {
        return NULL;
    }

    Node* curr = list->head;

    while (curr != NULL && cmp_func(curr->data, data) != 0) {
        curr = curr->next;
    }

    if (curr == NULL) {
        return NULL;
    }

    return curr->data;
}

void dllist_foreach(DLList *list, void(*func)(void*, void*), void *arg2) {
    if (list == NULL || list->size == 0 || func == NULL) {
        return;
    }

    Node *curr = list->head;

    while (curr != NULL) {
        func(curr->data, arg2);
        curr = curr->next;
    }
}

void dllist_clear(DLList *list, void(*free_func)(void*, void*), void *arg2) {
    if (list == NULL || list->size == 0 || free_func == NULL) {
        return;
    }

    Node *prev, *curr = list->head;

    while (curr != NULL) {
        prev = curr;
        curr = curr->next;

        free_func(prev->data, arg2);
        free(prev);
    }

    list->head = NULL;
    list->tail = NULL;
}
