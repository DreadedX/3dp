#include "flux/flux.h"

/** @brief List of all the loaded container */
Array<flux::Flux*> files;

/** @brief Amount of loaded files */
int count = 0;

/** @brief Hash map used to make sure we are not loading FLX files more than once */
std::map<std::string, flux::FileLoad*> map;

void flux::load() {

	/** @todo Make it find all flux files automatically */
	count = 2;
	std::string fileNames[] = {"core.flx", "demo.flx"};

	for (int i = 0; i < count; i++) {

		flux::Flux *file = new flux::Flux;
		files.add(file);

		file->load(fileNames[i]);
	}

	for (flux::Flux *file : files) {

		for (uint i = 0; i < file->indexSize; i++) {

			print::d("Asset: %s", file->index[i].name.c_str());
		}
	}
}

/** @todo Should the file be closed again */
void flux::Flux::load(std::string name) {

	this->name = name;
	valid = true;
	fileHandle = fopen(name.c_str(), "rb");

	if (fileHandle != nullptr) {

		fseek(fileHandle, 0, SEEK_END);
		ulong fileSize = ftell(fileHandle) - 4;
		fseek(fileHandle, 0, SEEK_SET);

		uint adler = adler32(0L, Z_NULL, 0);

		uint length = 1024;

		bool shouldContinue = true;

		while(shouldContinue) {

			ulong currentPosition = ftell(fileHandle);

			if (fileSize - currentPosition < length) {

				length = static_cast<uint>(fileSize - currentPosition);

				shouldContinue = false;
			}

			byte *buffer = new byte[length];

			fread(buffer, sizeof(byte), length, fileHandle);
			adler = adler32(adler, buffer, length);

			delete[] buffer;
		}

		uint checksum;
		fread(&checksum, sizeof(byte), sizeof(uint), fileHandle);

		fseek(fileHandle, 0, SEEK_SET);

		byte *header = new byte[6];
		fread(header, sizeof(byte), 4, fileHandle);
		// Make string null terminated
		header[4] = 0x00;
		header[5] = 0x00;

		std::string headerString(reinterpret_cast<const char*>(header));
		delete[] header;

		if (headerString == "FLX1" && checksum == adler)  {

			fread(&indexSize, sizeof(byte), sizeof(uint), fileHandle);
			print::d("FLX Container '%s' contains %i files", name.c_str(), indexSize);
			this->index = new FileLoad[indexSize];

			for (uint i = 0; i < indexSize; ++i) {

				// Read fileName
				byte nameSize = 0;
				fread(&nameSize, sizeof(byte), sizeof(byte), fileHandle);
				char *name = new char[nameSize];
				fread(name, sizeof(byte), nameSize, fileHandle);
				index[i].name = std::string(name, nameSize);
				delete[] name;

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

			print::d("FLX Container '%s' indexed", name.c_str());
			valid = true;

			return;
		}

		if (checksum != adler) {

			print::w("'%s' is corrupted", name.c_str());
		} else {
			
			print::w("'%s' is not a valid FLX1 file", name.c_str());
		}
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

/** @todo addNullTerminator should be removed */
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
	}

	files.clear();

	map.clear();
	load();
}
