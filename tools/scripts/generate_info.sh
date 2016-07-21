echo "#include \"flare/flare.h\"

void flare::info::print() {

	print::d(\"%-12s %s %s\", \"Compiled on:\", __DATE__, __TIME__);
	print::d(\"%-12s %s\", \"Version:\", \"$(git describe --always --dirty=-modified)\");
	print::d(\"%-12s %i\", \"C++ Version:\", __cplusplus);
}" > $1
