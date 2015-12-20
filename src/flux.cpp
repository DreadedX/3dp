#include "flare/flare.h"

flare::flux::Flux *files = nullptr;
int count = 0;

std::map<std::string, flare::flux::File*> map;

void flare::flux::load() {

    // TODO: Make it find all flux files automatically
    count = 1;
    std::string fileNames[] = {"base.flx"};
    files = new Flux[count];

    for (int i = 0; i < count; i++) {

	// files[i].setMap(&map);

	if (files[i].load(fileNames[i])) {

	    log::d("File indexed: '%s'", files[i].name.c_str());
	} else {

	    log::w("Failed to open: '%s'", files[i].name.c_str());
	}
    }
}

bool flare::flux::Flux::load(std::string name) {

    this->name = name;
    fileHandle = fopen(name.c_str(), "rb");

    if (fileHandle != nullptr) {

	byte *header = new byte[4];
	fread(header, sizeof(byte), 4, fileHandle);
	if (!strcmp(reinterpret_cast<const char*>(header), "FLX0"))  {

	    fread(&indexSize, sizeof(byte), sizeof(uint), fileHandle);
	    log::d("File count: %i", indexSize);
	    this->index = new File[indexSize];

	    for (uint i = 0; i < indexSize; ++i) {
	        
		// Read fileName
		byte nameSize = 0;
		fread(&nameSize, sizeof(byte), sizeof(byte), fileHandle);
		char *name = new char[nameSize];
		fread(name, sizeof(byte), nameSize, fileHandle);
		index[i].name = std::string(name, nameSize);

		// Read extra data
		fread(&index[i].extraSize, sizeof(byte), sizeof(uint), fileHandle);
		index[i].extra = new byte[index[i].extraSize];
		fread(index[i].extra, sizeof(byte), index[i].extraSize, fileHandle);

		// Read data size and location
		fread(&index[i].dataSize, sizeof(byte), sizeof(luint), fileHandle);
		fread(&index[i].compressedDataSize, sizeof(byte), sizeof(luint), fileHandle);
		fread(&index[i].dataLocation, sizeof(byte), sizeof(luint), fileHandle);

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
		log::d("Compressed size: %lu", index[i].compressedDataSize);
		log::d("Location: %lu", index[i].dataLocation);

		map[index[i].name] = &index[i];
		index[i].parent = this;
	    }

	    printf("\n");

	    this->open = true;
	    return true;
	}
    }

    return false;
}

flare::flux::File *flare::flux::get(std::string name) {

    if (map.find(name) != map.end()) {

	return map[name];
    }

    log::e("Asset '%s' not found", name.c_str());
    return nullptr;
}

byte *flare::flux::File::get() {

    log::d("Loading asset: '%s'", name.c_str());

    byte *compressedData = new byte[compressedDataSize];
    fseek(parent->fileHandle, dataLocation, SEEK_SET);
    fread(compressedData, sizeof(byte), compressedDataSize, parent->fileHandle);

    byte *data = new byte[dataSize];
    int result = uncompress(data, &dataSize, compressedData, compressedDataSize);
    if (result != Z_OK) {

	log::e("Uncompression of '%s' failed (%i)", name.c_str(), result);
    }

    delete[] compressedData;

    return data;
}

void flare::flux::close() {

    for (int i = 0; i < count; ++i) {
        
	files[i].close();
    }
    delete[] files;
}

void flare::flux::Flux::close() {

    if (fileHandle != nullptr) {

	fclose(fileHandle);
    }
    if (index != nullptr) {

	delete[] index;
	index = nullptr;
    }
    open = false;
}
