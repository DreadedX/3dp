#ifndef FLUX_H
#define FLUX_H

#include <vector>
#include <cstring>
#include <map>
#include <assert.h>

#include <zlib.h>

#include "extra/extra.h"

namespace flux {

    struct Flux;

    struct File {

	std::string name = "";

	uint extraSize = 0;
	byte *extra = nullptr;

	uint dataSize = 0;
	uint compressedDataSize = 0;
	llu dataLocation = 0;

	Flux *parent;

	bool inUse = false;

	byte *get(bool addNullTerminator = false);
    };

    struct Flux {

	std::string name = "";
	FILE *fileHandle = nullptr;
	uint indexSize = 0;
	File *index = nullptr;

	bool valid = true;

	void load(std::string name);
	void close();
    };

    void load();
    File *get(std::string name);
    void close();

    void free();
    void reload();
}

#endif

