#include <unordered_map>

#include <zlib.h>

#include "extra/allocator.h"
#include "flux/file.h"
#include "flux/read.h"

/** @brief List of all the loaded container */
// Array<flux::Flux*> files;
#include <vector>
std::vector<flux::Container*> containers;

/** @brief Amount of loaded files */
int count = 0;

/** @brief Hash map used to make sure we are not loading FLX files more than once */
std::unordered_map<std::string, flux::File*> map;

Allocator *flux_allocator = nullptr;

void flux::read::init(Allocator *_allocator) {

	flux_allocator = _allocator;
}

void flux::read::load() {

	/** @todo Make it find all flux files automatically */
	count = 2;
	std::string fileNames[] = {"core.flx", "demo.flx"};

	for (int i = 0; i < count; i++) {

		flux::Container *container = allocator::make_new<flux::Container>(*flux_allocator);
		container->load(fileNames[i]);

		containers.push_back(container);
	}

	for (flux::Container *container : containers) {

		for (uint i = 0; i < container->indexSize; i++) {

			print_d("Asset: %s", container->index[i].name);
		}
	}
}

/** @todo Should the file be closed again */
void flux::Container::load(std::string name) {

	this->name = name;
	valid = true;
	fileHandle = fopen(name.c_str(), "rb");

	if (fileHandle != nullptr) {

		fseek(fileHandle, 0, SEEK_END);
		ulong fileSize = ftell(fileHandle) - 4;
		fseek(fileHandle, 0, SEEK_SET);

		uint adler = adler32(0L, Z_NULL, 0);

		uint bufferSize = BUFFER_SIZE;

		bool shouldContinue = true;

		while(shouldContinue) {

			ulong currentPosition = ftell(fileHandle);

			if (fileSize - currentPosition < bufferSize) {

				bufferSize = static_cast<uint>(fileSize - currentPosition);

				shouldContinue = false;
			}

			// byte *buffer = new byte[bufferSize];
			byte *buffer = allocator::make_new_array<byte>(*flux_allocator, bufferSize);

			fread(buffer, sizeof(byte), bufferSize, fileHandle);
			adler = adler32(adler, buffer, bufferSize);

			// delete[] buffer;
			allocator::make_delete_array<byte>(*flux_allocator, buffer);
		}

		uint checksum;
		fread(&checksum, sizeof(byte), sizeof(uint), fileHandle);

		fseek(fileHandle, 0, SEEK_SET);

		byte *header = allocator::make_new_array<byte>(*flux_allocator, 6);
		fread(header, sizeof(byte), 4, fileHandle);
		// Make string null terminated
		header[4] = 0x00;
		header[5] = 0x00;

		std::string headerString(reinterpret_cast<const char*>(header));
		allocator::make_delete_array<byte>(*flux_allocator, header);

		if (headerString == "FLX1" && checksum == adler)  {

			fread(&indexSize, sizeof(byte), sizeof(uint), fileHandle);
			print_d("FLX Container '%s' contains %i files", name, indexSize);
			// this->index = new FileLoad[indexSize];
			this->index = allocator::make_new_array<File>(*flux_allocator, indexSize);

			for (uint i = 0; i < indexSize; ++i) {

				// Read fileName
				byte nameSize = 0;
				fread(&nameSize, sizeof(byte), sizeof(byte), fileHandle);
				// char *name = new char[nameSize];
				char *name = allocator::make_new_array<char>(*flux_allocator, nameSize);
				fread(name, sizeof(byte), nameSize, fileHandle);
				index[i].name = std::string(name, nameSize);
				// delete[] name;
				allocator::make_delete_array<char>(*flux_allocator, name);

				// Read data size and location
				fread(&index[i].dataSize, sizeof(byte), sizeof(uint), fileHandle);
				fread(&index[i].compressedDataSize, sizeof(byte), sizeof(uint), fileHandle);
				fread(&index[i].dataLocation, sizeof(byte), sizeof(ulong), fileHandle);

				// printf("\n");
				// log::d("- - - - ASSET - - - -");
				// log::d("Name: %s", index[i].name);
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

			print_d("FLX Container '%s' indexed", name);
			valid = true;

			return;
		}

		if (checksum != adler) {

			print_w("'%s' is corrupted", name);
		} else {
			
			print_w("'%s' is not a valid FLX1 file", name);
		}
		return;
	}
	print_e("Failed to open: '%s'", name);
	exit(-1);
}

flux::File *flux::read::get(std::string name) {

	if (map.find(name) != map.end()) {

		return map[name];
	}

	print_e("Asset '%s' not found", name);
	exit(-1);
}

/** @todo addNullTerminator should be removed */
byte *flux::File::load() {

	print_d("Loading file: '%s'", name);

	// byte *compressedData = new byte[compressedDataSize];
	byte *compressedData = allocator::make_new_array<byte>(*flux_allocator, compressedDataSize);
	fseek(parent->fileHandle, dataLocation, SEEK_SET);
	fread(compressedData, sizeof(byte), compressedDataSize, parent->fileHandle);

	byte *data = allocator::make_new_array<byte>(*flux_allocator, dataSize);

	uLongf tempDataSize = dataSize; 
	int result = uncompress(data, &tempDataSize, compressedData, compressedDataSize);
	assert(dataSize == tempDataSize);
	if (result != Z_OK) {

		print_e("Uncompression of '%s' failed (%i)", name, result);
		exit(-1);
	}

	allocator::make_delete_array<byte>(*flux_allocator, compressedData);

	return data;
}

void flux::read::close() {

	for (Container *container : containers) {

		container->close();

		// delete files[i];
		allocator::make_delete<flux::Container>(*flux_allocator, *container);
	}
}

void flux::Container::close() {

	if (fileHandle != nullptr) {

		fclose(fileHandle);
		fileHandle = nullptr;
	}
	if (index != nullptr) {

		// delete[] index;
		allocator::make_delete_array(*flux_allocator, index);
		index = nullptr;
	}
}

void flux::read::reload() {

	close();

	print_d("Removing old asset list");

	for (uint i = 0; i < containers.size(); ++i) {

		print_d("Freeing memory");
		containers[i]->close();
	}

	containers.clear();

	map.clear();
	load();
}
