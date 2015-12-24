#include "flux/flux.h"

std::vector<flux::Flux*> files;
int count = 0;

std::map<std::string, flux::FileLoad*> map;

void flux::load() {

    // TODO: Make it find all flux files automatically
    count = 1;
    std::string fileNames[] = {"base.flx"};

    for (int i = 0; i < count; i++) {

	flux::Flux *file = new flux::Flux;
	files.push_back(file);

	file->load(fileNames[i]);
    }
}

void flux::Flux::load(std::string name) {

    // static_assert(sizeof(luint) == 8, "luint is not 8 bytes");

    this->name = name;
    fileHandle = fopen(name.c_str(), "rb");

    if (fileHandle != nullptr) {

	byte *header = new byte[6];
	fread(header, sizeof(byte), 4, fileHandle);
	header[4] = 0x00;
	header[5] = 0x00;
	// TODO: This does not work on windows, propably because it is not null terminated
	if (std::string(reinterpret_cast<const char*>(header)) == "FLX0")  {

	    fread(&indexSize, sizeof(byte), sizeof(uint), fileHandle);
	    print::d("File count: %i", indexSize);
	    this->index = new FileLoad[indexSize];

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
		fread(&index[i].dataSize, sizeof(byte), sizeof(uint), fileHandle);
		fread(&index[i].compressedDataSize, sizeof(byte), sizeof(uint), fileHandle);
		fread(&index[i].dataLocation, sizeof(byte), sizeof(llu), fileHandle);

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

	    print::d("File indexed: '%s'", name.c_str());
	    valid = true;
	    return;
	}
	print::e("'%s' is not a FLX0 file", name.c_str());
	return;
    }
    print::e("Failed to open: '%s'", name.c_str());
    exit(-1);
}

flux::FileLoad *flux::get(std::string name) {

    if (map.find(name) != map.end()) {

	return map[name];
    }

    print::e("Asset '%s' not found", name.c_str());
    exit(-1);
}

byte *flux::FileLoad::get(bool addNullTerminator) {

    print::d("Loading asset: '%s'", name.c_str());

    byte *compressedData = new byte[compressedDataSize];
    fseek(parent->fileHandle, dataLocation, SEEK_SET);
    fread(compressedData, sizeof(byte), compressedDataSize, parent->fileHandle);

    byte *data = nullptr;
    if (!addNullTerminator) {

	data = new byte[dataSize];
    } else {

	data = new byte[dataSize+1];
	data[dataSize] = 0x00;
    }
    uLongf tempDataSize = dataSize;
    int result = uncompress(data, &tempDataSize, compressedData, compressedDataSize);
    assert(dataSize == tempDataSize);
    if (addNullTerminator) {

	print::d("%x", data[dataSize]);
    }
    if (result != Z_OK) {

	print::e("Uncompression of '%s' failed (%i)", name.c_str(), result);
	exit(-1);
    }

    delete[] compressedData;

    return data;
}

void flux::close() {

    for (int i = 0; i < count; ++i) {
        
	files[i]->close();
    }
}

// TODO: Check for memory leaks
void flux::Flux::close() {

    if (fileHandle != nullptr) {

	fclose(fileHandle);
    }
    if (index != nullptr) {

	delete[] index;
	index = nullptr;
    }
}

void flux::free() {

    for (uint i = 0; i < files.size(); ++i) {
	
	bool remove = true;

	if (!files[i]->valid) {

	    for (uint j = 0; j < files[i]->indexSize; ++j) {

		if (files[i]->index[j].inUse) {

		    remove = false;
		}
	    }

	    if (remove) {

		print::d("Freeing memory");
		files[i]->close();
		delete files[i];
		files.erase(files.begin()+i);
	    }
	}
    } 
}

void flux::reload() {

    print::d("Reloading assets");

    for (uint i = 0; i < files.size(); ++i) {

	files[i]->valid = false;
    }

    map.clear();
    load();
}
