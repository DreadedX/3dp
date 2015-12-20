#ifndef FLUX_H
#define FLUX_H

namespace flare {
    namespace flux {

	struct File {

	    std::string name = "";

	    uint extraSize = 0;
	    byte *extra = nullptr;

	    luint dataSize = 0;
	    luint dataLocation = 0;
	};

	struct Flux {

	    std::string name = "";
	    FILE *file = nullptr;
	    uint indexSize = 0;
	    File *index = nullptr;

	    bool open = false;

	    bool load(std::string name);
	    void close();
	};

	void load();
	void close();
    }
}

#endif

