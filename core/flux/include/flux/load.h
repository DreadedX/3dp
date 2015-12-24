#ifndef FILE_H
#define FILE_H

namespace flux {

    struct Flux;

    struct FileLoad : FileBase {

	Flux *parent;

	bool inUse = false;

	byte *get(bool addNullTerminator = false);
    };

    struct Flux {

	std::string name = "";
	FILE *fileHandle = nullptr;
	uint indexSize = 0;
	FileLoad *index = nullptr;

	bool valid = true;

	void load(std::string name);
	void close();
    };

    void load();
    FileLoad *get(std::string name);
    void close();

    void free();
    void reload();
}

#endif

