#ifndef FLUX_H
#define FLUX_H

namespace flare {
    namespace flux {

	struct Flux;

	struct File {

	    std::string name = "";

	    uint extraSize = 0;
	    byte *extra = nullptr;

	    luint dataSize = 0;
	    luint compressedDataSize = 0;
	    luint dataLocation = 0;

	    Flux *parent;

	    bool inUse = false;

	    byte *get();
	};

	struct Flux {

	    std::string name = "";
	    FILE *fileHandle = nullptr;
	    uint indexSize = 0;
	    File *index = nullptr;

	    bool valid = true;

	    bool load(std::string name);
	    void close();
	};

	void load();
	File *get(std::string name);
	void close();

	void free();
	void reload();
    }
}

#endif

