#include "flux/flux.h"

/** @brief List of all the loaded container */
std::vector<flux::Flux*> files;

/** @brief Amount of loaded files */
int count = 0;

/** @brief Hash map used to make sure we are not loading FLX files more than once */
std::map<std::string, flux::FileLoad*> map;

void flux::load() {

	/** @todo Make it find all flux files automatically */
	count = 1;
	std::string fileNames[] = {"base.flx"};

	for (int i = 0; i < count; i++) {

		flux::Flux *file = new flux::Flux;
		files.push_back(file);

		file->load(fileNames[i]);
	}
}

void flux::Flux::load(std::string name) {

	this->name = name;
	valid = true;
	fileHandle = fopen(name.c_str(), "rb");

	if (fileHandle != nullptr) {

		byte *header = new byte[6];
		fread(header, sizeof(byte), 4, fileHandle);
		header[4] = 0x00;
		header[5] = 0x00;
		/** @bug This does not work on windows, propably because it is not null terminated (Is this still the case when using MinGW?) */
		std::string headerString(reinterpret_cast<const char*>(header));
		delete[] header;
		if (headerString == "FLX0")  {

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
				delete[] name;

				// Read extra data
				fread(&index[i].extraSize, sizeof(byte), sizeof(uint), fileHandle);
				index[i].extra = new byte[index[i].extraSize];
				fread(index[i].extra, sizeof(byte), index[i].extraSize, fileHandle);

				// Read data size and location
				fread(&index[i].dataSize, sizeof(byte), sizeof(uint), fileHandle);
				fread(&index[i].compressedDataSize, sizeof(byte), sizeof(uint), fileHandle);
				fread(&index[i].dataLocation, sizeof(byte), sizeof(ulong), fileHandle);

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

	print::d("Loading file: '%s'", name.c_str());

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
		delete files[i];
	}
}

/** .todo Check this for memory leaks (Pretty sure there are non) */
void flux::Flux::close() {

	if (fileHandle != nullptr) {

		// @todo This causes a SEGFAULT ?? (Is this even needed)
		// fclose(fileHandle);
	}
	if (index != nullptr) {

		for (uint i = 0; i < indexSize; i++) {

			delete[] index[i].extra;
		}

		delete[] index;
		index = nullptr;
	}
}

void flux::reload() {

	close();

	print::d("Removing old asset list");

	for (uint i = 0; i < files.size(); ++i) {

		print::d("Freeing memory");
		files[i]->close();
		files.erase(files.begin()+i);
	}

	map.clear();
	load();
}
