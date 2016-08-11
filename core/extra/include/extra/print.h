#ifndef PRINT_H
#define PRINT_H

#include <string>
#include <cstring>

#include "fmt/format.h"

/** @brief Print functions that adds logging */
namespace print {

	enum Color { BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE, RESET = 9};

	/** Write a normal message to the log
		@param output - The file to output to
		@param color - The color the log entry should be in
		@param name - Name of the file this function is called from
		@param line - Line number this function is called from
		@param format, ... - This works the same as printf */
	void log(std::FILE *output, print::Color color, const char *tag, const char *name, int line, const char *format, fmt::ArgList args);

	//** Defines a macro for getting the filename excluding the path */
	#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
	#define FILENAME_LENGTH 30

	FMT_VARIADIC(void, log, std::FILE*, print::Color, const char*, const char*, int, const char*)
	/** Wraps print::logm so it automatically fills in the filename and linenumber for you */
	#define print_m(format, ...) print::log(stdout, print::RESET, "MESSAGE", __FILENAME__, __LINE__, format, ##__VA_ARGS__)
	/** Wraps print::logd so it automatically fills in the filename and linenumber for you */
	#ifndef NDEBUG
		#define print_d(format, ...) print::log(stdout, print::BLUE, "DEBUG", __FILENAME__, __LINE__, format, ##__VA_ARGS__)
	#else
		#define print_d(format, ...)
	#endif
	/** Wraps print::logw so it automatically fills in the filename and linenumber for you */
	#define print_w(format, ...) print::log(stderr, print::YELLOW, "WARNING", __FILENAME__, __LINE__, format, ##__VA_ARGS__)
	/** Wraps print::loge so it automatically fills in the filename and linenumber for you */
	#define print_e(format, ...) print::log(stderr, print::RED, "ERROR", __FILENAME__, __LINE__, format, ##__VA_ARGS__)
}

#endif

