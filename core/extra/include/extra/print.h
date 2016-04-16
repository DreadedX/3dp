#ifndef PRINT_H
#define PRINT_H

namespace print {

    void logm(const char *name, int line, const char *format, ...);
    void logd(const char *name, int line, const char *format, ...);
    void logw(const char *name, int line, const char *format, ...);
    void loge(const char *name, int line, const char *format, ...);

    std::string format(const char *fmt, ...);
}

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define FILENAME_LENGTH 30
#define m(format, ...) logm(__FILENAME__, __LINE__, format, ##__VA_ARGS__)
#define d(format, ...) logd(__FILENAME__, __LINE__, format, ##__VA_ARGS__)
#define w(format, ...) logw(__FILENAME__, __LINE__, format, ##__VA_ARGS__)
#define e(format, ...) loge(__FILENAME__, __LINE__, format, ##__VA_ARGS__)

#endif

