#ifndef CORE_H
#define CORE_H

#include <stdint.h>
#include <errno.h>

#define ERRNO_STR strerror(errno)

typedef struct color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

void error(const char *format, ...);
void logg(const char *format, ...);
void die(const char *format, ...);

#endif //CORE_H
