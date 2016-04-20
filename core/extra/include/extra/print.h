#ifndef PRINT_H
#define PRINT_H

/** @brief Print functions that adds logging */
namespace print {

	/** Write a normal message to the log
		@param name - Name of the file this function is called from
		@param line - Line number this function is called from
		@param format, ... - This works the same as printf
		@note Use print::m(const char *format, ...) instead */
    void logm(const char *name, int line, const char *format, ...);
	/** Write a debug message to the log
		@param name - Name of the file this function is called from
		@param line - Line number this function is called from
		@param format, ... - This works the same as printf
		@note Use print::d(const char *format, ...) instead */
    void logd(const char *name, int line, const char *format, ...);
	/** Write a warning to the log
		@param name - Name of the file this function is called from
		@param line - Line number this function is called from
		@param format, ... - This works the same as printf
		@note Use print::w(const char *format, ...) instead */
    void logw(const char *name, int line, const char *format, ...);
	/** Write an error to the log
		@param name - Name of the file this function is called from
		@param line - Line number this function is called from
		@param format, ... - This works the same as printf
		@note Use print::e(const char *format, ...) instead */
    void loge(const char *name, int line, const char *format, ...);

	/** Formats a string the same way printf formats a string
		@todo Pretty sure this is not used anywhere or even that useful, so it should propably be removed */
    std::string format(const char *fmt, ...);

	//** Defines a macro for getting the filename excluding the path */
	#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

	/** Wraps print::logm so it automatically fills in the filename and linenumber for you */
	#define m(format, ...) logm(__FILENAME__, __LINE__, format, ##__VA_ARGS__)
	/** Wraps print::logd so it automatically fills in the filename and linenumber for you */
	#define d(format, ...) logd(__FILENAME__, __LINE__, format, ##__VA_ARGS__)
	/** Wraps print::logw so it automatically fills in the filename and linenumber for you */
	#define w(format, ...) logw(__FILENAME__, __LINE__, format, ##__VA_ARGS__)
	/** Wraps print::loge so it automatically fills in the filename and linenumber for you */
	#define e(format, ...) loge(__FILENAME__, __LINE__, format, ##__VA_ARGS__)
}

#endif

