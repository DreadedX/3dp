#include "extra/print.h"

/** Define the max filename length for spacing */
#define FILENAME_LENGTH 30

/** @brief Print a timestamp
  @param stdName - Where to write the timestamp to */
void printTime(FILE *stdName) {

	time_t rawtime;
	time(&rawtime);

	char *p = ctime(&rawtime);
	int len = strlen(p);

	fprintf(stdName, "%.*s  ", len - 1, p);
}

/** @todo This should return a const char* */
std::string print::format(const char *fmt, ...) {
	int size = 512;
	char *buffer = 0;
	buffer = new char[size];
	va_list vl;
	va_start(vl, fmt);
	int nsize = vsnprintf(buffer, size, fmt, vl);
	if(size <= nsize) {
		delete[] buffer;
		buffer = new char[nsize+1];
		vsnprintf(buffer, size, fmt, vl);
	}
	std::string ret(buffer);
	va_end(vl);
	delete[] buffer;
	buffer = nullptr;
	return ret;
}

void print::logm(const char *name, int line, const char *format, ...) {

	va_list args;
	va_start(args, format);
	fprintf(stdout, "\033[39m%-9s" , "MESSAGE");
	printTime(stdout);
	fprintf(stdout, "%-*s", FILENAME_LENGTH, print::format("%s:%i", name, line).c_str());
	vfprintf(stdout, format, args);
	va_end(args);
	fprintf(stdout, "\033[39m\n");
}

void print::logd(const char *name, int line, const char *format, ...) {

#ifndef NDEBUG
	va_list args;
	va_start(args, format);
	fprintf(stdout, "\033[37m%-9s" , "DEBUG");
	printTime(stdout);
	fprintf(stdout, "%-*s", FILENAME_LENGTH, print::format("%s:%i", name, line).c_str());
	vfprintf(stdout, format, args);
	va_end(args);
	fprintf(stdout, "\033[39m\n");
#endif
}

void print::logw(const char *name, int line, const char *format, ...) {

	va_list args;
	va_start(args, format);
	fprintf(stderr, "\033[33m%-9s" , "WARNING");
	printTime(stderr);
	fprintf(stderr, "%-*s", FILENAME_LENGTH, print::format("%s:%i", name, line).c_str());
	vfprintf(stderr, format, args);
	va_end(args);
	fprintf(stderr, "\033[39m\n");
}

void print::loge(const char *name, int line, const char *format, ...) {

	va_list args;
	va_start(args, format);
	fprintf(stderr, "\033[31m%-9s" , "ERROR");
	printTime(stderr);
	fprintf(stderr, "%-*s", FILENAME_LENGTH, print::format("%s:%i", name, line).c_str());
	vfprintf(stderr, format, args);
	va_end(args);
	fprintf(stderr, "\033[39m\n");
}
