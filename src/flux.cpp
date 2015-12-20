#include "flare/flare.h"

std::vector<flare::flux::Flux*> files;
int count = 0;

std::map<std::string, flare::flux::File*> map;

void flare::flux::load() {

    // TODO: Make it find all flux files automatically
    count = 1;
    std::string fileNames[] = {"base.flx"};

    for (int i = 0; i < count; i++) {

	flux::Flux *file = new flux::Flux;
	files.push_back(file);

	if (file->load(fileNames[i])) {

	    log::d("File indexed: '%s'", file->name.c_str());
	} else {

	    log::w("Failed to open: '%s'", file->name.c_str());
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
		// printf("\n");
		// log::d("- - - - ASSET - - - -");
		// log::d("Name: %s", index[i].name.c_str());
		// printf("[DEBUG] Extra: ");
		// for (uint j = 0; j < index[i].extraSize; j++) {
		//     printf("%x ", index[i].extra[j]);
		// }
		// printf("\n");
		// log::d("Size: %lu", index[i].dataSize);
		// log::d("Compressed size: %lu", index[i].compressedDataSize);
		// log::d("Location: %lu", index[i].dataLocation);

		map[index[i].name] = &index[i];
		index[i].parent = this;
	    }

	    valid = true;
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
        
	files[i]->close();
    }
}

// TODO: Check for memory leaks
void flare::flux::Flux::close() {

    if (fileHandle != nullptr) {

	fclose(fileHandle);
    }
    if (index != nullptr) {

	delete[] index;
	index = nullptr;
    }
}

void flare::flux::free() {

    for (uint i = 0; i < files.size(); ++i) {
	
	bool remove = true;

	if (!files[i]->valid) {

	    for (uint j = 0; j < files[i]->indexSize; ++j) {

		if (files[i]->index[j].inUse) {

		    remove = false;
		}
	    }

	    if (remove) {

		log::d("Freeing memory");
		files[i]->close();
		delete files[i];
		files.erase(files.begin()+i);
	    }
	}
    } 
}

void flare::flux::reload() {

    log::d("Reloading assets");

    for (uint i = 0; i < files.size(); ++i) {

	files[i]->valid = false;
    }

    map.clear();
    load();
}
