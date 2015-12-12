#ifndef FLUX_H
#define FLUX_H

namespace flux {

    struct File {
	
	unsigned char nameSize = 0;
	const char *name = "";

	int extraSize = 0;
	unsigned char *extra = nullptr;

	int dataSize = 0;
	int dataLocation = 0;
	unsigned char *data = nullptr;
    };
}

#endif

