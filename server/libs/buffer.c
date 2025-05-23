#include "buffer.h"
#include "utils.h"

#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void move_to_front(Buffer *b);
bool has_space(Buffer *b, size_t size);
bool resize(Buffer *b);

Buffer* buffer_create() {
    Buffer *b = malloc(sizeof(Buffer));
    if (b == NULL) {
        return NULL;
    }

    b->buffer_begin = malloc(INIT_BUFFER_SIZE * sizeof(uint8_t));
    if (b->buffer_begin == NULL) {
        free(b);
        return NULL;
    }

    b->data_begin = b->data_end = b->buffer_begin;
    b->buffer_end = b->buffer_begin + INIT_BUFFER_SIZE - 1;

    memset(b->buffer_begin, 0, INIT_BUFFER_SIZE);

    return b;
}

void buffer_free(Buffer **buffer) {
    assert(buffer != NULL);

    if (*buffer == NULL) {
        return;
    }

    free((*buffer)->buffer_begin);
    free(*buffer);
    *buffer = NULL;
}

void buffer_consume(Buffer *b, size_t size) {
    assert(b != NULL);

    if (b->data_end - b->data_begin < size || b->data_begin + size > b->buffer_end) {
        error("Trying to consume too much data.\n");
        return;
    }

    b->data_begin += size;

    if (b->data_begin == b->data_end) {
        b->data_begin = b->data_end = b->buffer_begin;
    }
}

void buffer_append(Buffer *b, const uint8_t *data, size_t size) {
    assert(b != NULL && data != NULL);
    if (size == 0) {
        return;
    }

    while (!has_space(b, size)) {
        resize(b);
    }

    memcpy(b->data_end, data, size);
    b->data_end += size;
}

void buffer_print(const Buffer *b) {
    assert(b != NULL);

    fwrite(b->data_begin, 1, b->data_end - b->data_begin, stdout);
    printf("\n");
}

bool buffer_isempty(const Buffer *b) {
    if (b == NULL) {
        return true;
    }

    return b->data_begin == b->data_end;
}

void move_to_front(Buffer *b) {
    assert(b != NULL);

    if (b->data_begin == b->buffer_begin) {
        return;
    }

    size_t data_size = b->data_end - b->data_begin;
    memmove(b->buffer_begin, b->data_begin, data_size);

    b->data_begin = b->buffer_begin;
    b->data_end = b->data_begin + data_size;
}

bool has_space(Buffer *b, size_t size) {
    assert(b != NULL);

    if (b->data_end + size - 1 > b->buffer_end) {
        move_to_front(b);

        if (b->data_end + size - 1 > b->buffer_end) {
            return false;
        }
    }

    return true;
}

bool resize(Buffer *b) {
    assert(b != NULL);

    size_t curr_size = b->buffer_end - b->buffer_begin + 1;
    size_t data_begin_offset = b->data_begin - b->buffer_begin;
    size_t data_end_offset = b->data_end - b->buffer_begin;

    uint8_t *new_buf = realloc(b->buffer_begin, 2 * curr_size);
    if (new_buf == NULL) {
        return false;
    }

    b->buffer_begin = new_buf;
    b->buffer_end = new_buf + 2 * curr_size;
    b->data_begin = new_buf + data_begin_offset;
    b->data_end = new_buf + data_end_offset;

    return true;
}
