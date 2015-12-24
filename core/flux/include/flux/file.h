#ifndef LOAD_H
#define LOAD_H

namespace flux {

    struct FileBase {
	
	unsigned char nameSize = 0;
	std::string name = "";

	unsigned int extraSize = 0;
	unsigned char *extra = nullptr;

	uint dataSize = 0;
	uint compressedDataSize = 0;
	llu dataLocation = 0;
    };

}

#endif

