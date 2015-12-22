#ifndef PRINT_H
#define PRINT_H

namespace print {

    void m(const char *format, ...);
    void d(const char *format, ...);
    void w(const char *format, ...);
    void e(const char *format, ...);

    std::string format(const char *fmt, ...);
}

#endif

