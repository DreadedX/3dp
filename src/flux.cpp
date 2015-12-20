#include "flare/flare.h"

flare::flux::Flux *files = nullptr;
int count = 0;

// This function should create an index of all flux files
void flare::flux::load() {

    // TODO: Make it find all flux files automatically
    count = 1;
    std::string fileNames[] = {"base.flx"};
    files = new Flux[count];

    for (int i = 0; i < count; i++) {

	if (files[i].load(fileNames[i])) {

	    log::d("File indexed: '%s'", files[i].name.c_str());
	} else {

	    log::w("Failed to open: '%s'", files[i].name.c_str());
	}
    }
}

bool flare::flux::Flux::load(std::string name) {

    this->name = name;
    file = fopen(name.c_str(), "rb");

    if (file != nullptr) {

	byte *header = new byte[4];
	fread(header, sizeof(byte), 4, file);
	if (!strcmp(reinterpret_cast<const char*>(header), "FLX0"))  {

	    fread(&indexSize, sizeof(byte), sizeof(uint), file);
	    log::d("File count: %i", indexSize);
	    this->index = new File[indexSize];

	    for (uint i = 0; i < indexSize; ++i) {
	        
		// Read fileName
		byte nameSize = 0;
		fread(&nameSize, sizeof(byte), sizeof(byte), file);
		char *name = new char[nameSize];
		fread(name, sizeof(byte), nameSize, file);
		index[i].name = std::string(name, nameSize);

		// Read extra data
		fread(&index[i].extraSize, sizeof(byte), sizeof(uint), file);
		index[i].extra = new byte[index[i].extraSize];
		fread(index[i].extra, sizeof(byte), index[i].extraSize, file);

		// Read data size and location
		fread(&index[i].dataSize, sizeof(byte), sizeof(luint), file);
		fread(&index[i].dataLocation, sizeof(byte), sizeof(luint), file);

		// TODO: DO NOT USE PRINTF
		printf("\n");
		log::d("- - - - ASSET - - - -");
		log::d("Name: %s", index[i].name.c_str());
		printf("[DEBUG] Extra: ");
		for (uint j = 0; j < index[i].extraSize; j++) {
		    printf("%x ", index[i].extra[j]);
		}
		printf("\n");
		log::d("Size: %lu", index[i].dataSize);
		log::d("Location: %lu", index[i].dataLocation);
	    }

	    printf("\n");

	    this->open = true;
	    return true;
	}
    }

    return false;
}

void flare::flux::close() {

    for (int i = 0; i < count; ++i) {
        
	files[i].close();
    }
    delete[] files;
}

void flare::flux::Flux::close() {

    if (file != nullptr) {

	fclose(file);
    }
    if (index != nullptr) {

	delete[] index;
	index = nullptr;
    }
    open = false;
}
