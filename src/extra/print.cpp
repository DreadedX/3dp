#include "extra/extra.h"

void print::m(const char *format, ...) {

    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    fprintf(stdout, "\n");
}

void print::d(const char *format, ...) {

    va_list args;
    va_start(args, format);
    fprintf(stdout, "[DEBUG] ");
    vfprintf(stdout, format, args);
    va_end(args);
    fprintf(stdout, "\n");
}

void print::w(const char *format, ...) {

    va_list args;
    va_start(args, format);
    fprintf(stderr, "[WARNING] ");
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}

void print::e(const char *format, ...) {

    va_list args;
    va_start(args, format);
    fprintf(stderr, "[ERROR] ");
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}
