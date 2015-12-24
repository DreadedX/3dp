#ifndef FLUX_H
#define FLUX_H

namespace flux {

    struct File {
	
	unsigned char nameSize = 0;
	std::string name = "";

	unsigned int extraSize = 0;
	unsigned char *extra = nullptr;

	uint dataSize = 0;
	uint compressedDataSize = 0;
	llu dataLocation = 0;
	unsigned char *data = nullptr;
    };
}

#endif

