#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h> // for size_t
#include <stdint.h> // for uint8_t
#include <stdbool.h>

#define INIT_BUFFER_SIZE 1024
#define TEMP_BUFFER_SIZE 4096

typedef struct buffer {
    uint8_t *data_begin;
    uint8_t *data_end;
    uint8_t *buffer_begin;
    uint8_t *buffer_end;
} Buffer;

Buffer* buffer_create();
void    buffer_free(Buffer **buffer);
void    buffer_consume(Buffer *b, size_t size);
void    buffer_append(Buffer *b, const uint8_t* data, size_t size);
void    buffer_print(const Buffer *b);
bool    buffer_isempty(const Buffer *b);

#endif //BUFFER_H
