#include "flare/flare.h"

void flare::log::l(const char *format, ...) {

    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    fprintf(stdout, "\n");
}

void flare::log::d(const char *format, ...) {

    va_list args;
    va_start(args, format);
    fprintf(stdout, "[DEBUG] ");
    vfprintf(stdout, format, args);
    va_end(args);
    fprintf(stdout, "\n");
}

void flare::log::w(const char *format, ...) {

    va_list args;
    va_start(args, format);
    fprintf(stderr, "[WARNING] ");
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}

void flare::log::e(const char *format, ...) {

    va_list args;
    va_start(args, format);
    fprintf(stderr, "[ERROR] ");
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}
