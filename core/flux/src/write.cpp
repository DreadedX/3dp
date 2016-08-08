#include <zlib.h>

#include "extra/array.h"
#include "extra/print.h"

#include "flux/file.h"
#include "flux/write.h"

void flux::write::compress(Array<flux::File*> *files) {

	for (flux::File *file : *files) {

		file->nameSize = file->name.length();

		// Compress the data
		uint uncompressedDataSize = file->dataSize;
		byte *uncompressedData = file->data;
		// Temporary size
		uLongf compressedDataSize = file->dataSize + 1024;
		byte *compressedData = new byte[compressedDataSize];

		int result = compress2(compressedData, &compressedDataSize, uncompressedData, uncompressedDataSize, Z_BEST_SPEED);
		if (result != Z_OK) {
			print_e("Compression of '%s' failed (%i)", file->name.c_str(), result);
			exit(-1);
		}

		delete[] file->data;

		file->compressedDataSize = compressedDataSize;
		file->data = compressedData;
	}
}

uint flux::write::write(Array<flux::File*> *files, const char *containerName) {

	ulong totalSize = 0;
	ulong bytes_written = 0;
	// TODO: Determine the output file name based on the directory
	FILE *container = fopen(containerName, "w+b");

	ulong dataLocation = 4 + sizeof(uint);
	for (flux::File *file : *files) {

		assert(file->compressedDataSize != 0);

		dataLocation += sizeof(byte);
		dataLocation += file->nameSize;
		dataLocation += sizeof(uint);
		dataLocation += sizeof(uint);
		dataLocation += sizeof(ulong);
	}
	totalSize += dataLocation;
	for (flux::File *file : *files) {

		file->dataLocation = dataLocation;
		dataLocation += file->compressedDataSize;
	}

	uint count = files->size();
	/** @todo This needs to move into flux */
	bytes_written += fwrite("FLX1", sizeof(byte), 4, container);
	bytes_written += fwrite(&count, sizeof(byte), sizeof(uint), container);
	for (flux::File *file : *files) {

		bytes_written += fwrite(&file->nameSize, sizeof(byte), sizeof(byte), container);
		bytes_written += fwrite(file->name.c_str(), sizeof(byte), file->nameSize, container);
		bytes_written += fwrite(&file->dataSize, sizeof(byte), sizeof(uint), container);
		bytes_written += fwrite(&file->compressedDataSize, sizeof(byte), sizeof(uint), container);
		bytes_written += fwrite(&file->dataLocation, sizeof(byte), sizeof(ulong), container);
	}
	for (flux::File *file : *files) {

		bytes_written += fwrite(file->data, sizeof(byte), file->compressedDataSize, container);

		totalSize += file->compressedDataSize;
	}

	fseek(container, 0, SEEK_END);
	ulong fileSize = ftell(container);
	fseek(container, 0, SEEK_SET);

	uint adler = adler32(0L, Z_NULL, 0);

	uint bufferSize = BUFFER_SIZE;
	byte *buffer = nullptr;

	bool shouldContinue = true;

	while(shouldContinue) {

		ulong currentPosition = ftell(container);

		if (fileSize - currentPosition < bufferSize) {

			bufferSize = static_cast<uint>(fileSize - currentPosition);

			shouldContinue = false;
		}

		buffer = new byte[bufferSize];

		uint read = fread(buffer, sizeof(byte), bufferSize, container);
		assert(bufferSize == read);
		adler = adler32(adler, buffer, bufferSize);

		delete[] buffer;
		buffer = nullptr;
	}

	fseek(container, 0, SEEK_END);

	bytes_written += fwrite(&adler, sizeof(byte), sizeof(uint), container);
	totalSize += sizeof(uint);

	fclose(container);

	assert(bytes_written == totalSize);

	return totalSize;
}

