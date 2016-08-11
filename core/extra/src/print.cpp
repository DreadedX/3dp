#include "fmt/printf.h"
#include "extra/print.h"

void print::log(std::FILE *output, print::Color color, const char *tag, const char *name, int line, const char *format, fmt::ArgList args) {

	time_t rawtime;
	time(&rawtime);

	char *p = ctime(&rawtime);
	int len = strlen(p);

	char escape[] = "\x1b[30m";
	escape[3] = static_cast<char>('0' + color);
	std::fputs(escape, output);

	fmt::fprintf(output, "%-8s  ", tag);
	fmt::fprintf(output, "%.*s  ", len-1, p);
	#ifndef NDEBUG
		fmt::fprintf(output, "%-*s  ", FILENAME_LENGTH, std::string(name) + ":" + std::to_string(line));
	#endif
	fmt::fprintf(output, format, args);
	fmt::fprintf(output, "\n");

	std::fputs("\x1b[0m", output);
}
