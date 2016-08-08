#ifndef WRITE_H
#define WRITE_H

#include "extra/array.h"

namespace flux::write {

	#define BUFFER_SIZE 1024

	void compress(Array<flux::File*> *files);
	uint write(Array<flux::File*> *files, const char *containerName);
}

#endif

