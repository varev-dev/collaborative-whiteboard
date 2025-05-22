#include "utils.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void error(const char *format, ...) {
    va_list args;

    fprintf(stderr, "[ERROR]: ");

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

void logg(const char *format, ...) {
    va_list args;

    fprintf(stdout, "[LOG]: ");

    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}

void die(const char *format, ...) {
    va_list args;

    fprintf(stderr, "[EXIT]: ");

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    exit(EXIT_FAILURE);
}
