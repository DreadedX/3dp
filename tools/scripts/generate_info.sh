echo "#include <zlib.h>
#include \"extra/print.h\"
#include \"flare/info.h\"

void flare::info::print() {

print_d(\"%-12s %s %s @ %s\", \"Compiled on:\", __DATE__, __TIME__, \"$(hostname)\");
	print_d(\"%-12s %s\", \"Version:\", \"$(git describe --always --dirty=-modified)\");
	print_d(\"%-12s %i\", \"C++ version:\", __cplusplus);
	print_d(\"%-12s %s (%s)\", \"Zlib:\", ZLIB_VERSION, zlibVersion());
}" > $1
