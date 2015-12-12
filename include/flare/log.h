#ifndef LOG_H
#define LOG_H

namespace flare {
    namespace log {

	void l(const char *format, ...);
	void d(const char *format, ...);
	void w(const char *format, ...);
	void e(const char *format, ...);
    }
}

#endif

