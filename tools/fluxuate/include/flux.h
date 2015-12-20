#ifndef FLUX_H
#define FLUX_H

namespace flux {

    struct File {
	
	unsigned char nameSize = 0;
	std::string name = "";

	unsigned int extraSize = 0;
	unsigned char *extra = nullptr;

	long unsigned int dataSize = 0;
	long unsigned int compressedDataSize = 0;
	long unsigned int dataLocation = 0;
	unsigned char *data = nullptr;
    };
}

#endif

