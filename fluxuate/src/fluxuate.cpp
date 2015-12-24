#include "standard.h"

void getDir(std::string dir, std::vector<std::string> &files);
void getFile(std::string basePath, std::string fileName, flux::FileWrite *file);

int main(int argc, char* argv[]) {

    std::string dir;
    if (argc == 2) {

	dir = argv[1];
    } else {

	printf("Please specify an asset folder!\n");
	exit(-1);
    }
    std::vector<std::string> files = std::vector<std::string>();

    getDir(dir, files);
    uint count = files.size();

    flux::FileWrite *fluxFiles = new flux::FileWrite[count];

    for (uint i = 0; i < count; ++i) {

	getFile(dir, files[i].c_str(), &fluxFiles[i]);
    }

    llu totalSize = 0;
    llu bytes_written = 0;
    // TODO: Determine the output file name based on the directory
    FILE *file = fopen("base.flx", "wb");

    llu dataLocation = 4 + sizeof(uint);
    for (uint i = 0; i < count; i++) {

	dataLocation += sizeof(byte);
	dataLocation += fluxFiles[i].nameSize;
	dataLocation += sizeof(uint);
	dataLocation += fluxFiles[i].extraSize;
	dataLocation += sizeof(uint);
	dataLocation += sizeof(uint);
	dataLocation += sizeof(llu);
    }
    totalSize += dataLocation;
    for (uint i = 0; i < count; i++) {

	fluxFiles[i].dataLocation = dataLocation;
	dataLocation += fluxFiles[i].dataSize;
    }
    bytes_written += fwrite("FLX0", sizeof(byte), 4, file);
    bytes_written += fwrite(&count, sizeof(byte), sizeof(uint), file);
    for (uint i = 0; i < count; i++) {

	bytes_written += fwrite(&fluxFiles[i].nameSize, sizeof(byte), sizeof(byte), file);
	bytes_written += fwrite(fluxFiles[i].name.c_str(), sizeof(byte), fluxFiles[i].nameSize, file);
	bytes_written += fwrite(&fluxFiles[i].extraSize, sizeof(byte), sizeof(uint), file);
	bytes_written += fwrite(fluxFiles[i].extra, sizeof(byte), fluxFiles[i].extraSize, file);
	bytes_written += fwrite(&fluxFiles[i].dataSize, sizeof(byte), sizeof(uint), file);
	bytes_written += fwrite(&fluxFiles[i].compressedDataSize, sizeof(byte), sizeof(uint), file);
	bytes_written += fwrite(&fluxFiles[i].dataLocation, sizeof(byte), sizeof(llu), file);
    }
    for (uint i = 0; i < count; i++) {

	bytes_written += fwrite(fluxFiles[i].data, sizeof(byte), fluxFiles[i].dataSize, file);

	totalSize += fluxFiles[i].dataSize;
    }
    fclose(file);
    assert(bytes_written == totalSize);
    printf("Total data size: %llu\n", totalSize);

    // Preventing memory leaks
    for (uint i = 0; i < count; i++) {

	delete[] fluxFiles[i].data;
	fluxFiles[i].data = nullptr;
	delete[] fluxFiles[i].extra;
	fluxFiles[i].extra = nullptr;
    }
    delete[] fluxFiles;
}

void getDir(std::string dir, std::vector<std::string> &files) {

    DIR *dp;
    struct dirent *dirp;

    if ((dp = opendir(dir.c_str())) == NULL) {
	
	fprintf(stderr, "Error (%i) while opening %s\n", errno, dir.c_str());
	return;
    }

    while ((dirp = readdir(dp)) != NULL) {

	const char *name = dirp->d_name;
	if (strcmp(name, ".") && strcmp(name, "..")) {
	    files.push_back(std::string(dirp->d_name));
	}
    }

    closedir(dp);
}

void getFile(std::string basePath, std::string fileName, flux::FileWrite *file) {

    std::string extension;
    std::string baseName;
    int length = fileName.length();
    int lastPoint = fileName.find_last_of(".");

    // flux::File file;

    if (lastPoint != 0) {

	extension = fileName.substr(lastPoint+1, length);
	baseName = fileName.substr(0, lastPoint);
    } else {

	extension = "";
	baseName = fileName;
    }

    // TODO: This needs needs to go automatically (recursive directory)
    std::string assetName = "base/" + baseName;
    file->nameSize = assetName.size();
    file->name = assetName;

    // TODO: Make this based on the actual filePath
    std::string filePath = basePath + "/" + fileName;

    // Images are a special case
    // Now we don't need have to use libpng in the engine
    if (extension == "png") {

	png::Data data = png::read(filePath.c_str());

	file->extraSize = sizeof(int) * 2 + sizeof(byte);
	file->extra = new byte[file->extraSize];

	for (uint i = 0; i < sizeof(int); ++i) {

	    file->extra[i] = data.width >> (i*8);
	}
	for (uint i = sizeof(int); i < sizeof(int)*2; ++i) {

	    file->extra[i] = data.height >> (i*8);
	}
	file->extra[sizeof(int)*2] = data.bytesPerPixel;

	// file->data = data.pixels;
	file->dataSize = data.size;
	file->data = new byte[file->dataSize];
	for (int y = 0; y < data.height; y++) {
	    for (int x = 0; x < data.width*data.bytesPerPixel; x++) {

		file->data[x + y * (data.width*data.bytesPerPixel)] = data.pixels[x + (data.height-y-1) * (data.width*data.bytesPerPixel)];
	    }
	}
    }
    else {

	if (extension == "vertex" || extension == "fragment") {

	    std::string assetNameSpecial = assetName + "_" + extension;
	    file->nameSize = assetNameSpecial.size();
	    file->name = assetNameSpecial;
	}

	FILE *sourceFile = fopen(filePath.c_str(), "rb");
	assert(sourceFile != NULL);

	fseek(sourceFile, 0, SEEK_END);
	file->dataSize = ftell(sourceFile);
	fseek(sourceFile, 0, SEEK_SET);

	file->data = new byte[file->dataSize];
	llu bytes_read = fread(file->data, sizeof(byte), file->dataSize, sourceFile);

	// Make sure it read all the data
	assert(bytes_read == file->dataSize);
	fclose(sourceFile);
    }

    // Compress the data
    uint uncompressedDataSize = file->dataSize;
    byte *uncompressedData = file->data;
    // Temporary size
    uLongf compressedDataSize = file->dataSize + 1024;
    byte *compressedData = new byte[compressedDataSize];

    int result = compress(compressedData, &compressedDataSize, uncompressedData, uncompressedDataSize);
    if (result != Z_OK) {
	printf("Compression of '%s' failed (%i)\n", file->name.c_str(), result);
	exit(-1);
    }

    delete[] file->data;

    file->compressedDataSize = compressedDataSize;
    file->data = compressedData;
#if 0
    printf("- - - - ASSET - - - -\n");
    printf("Name: %s\n", file->name.c_str());
    printf("Data adress: %p\n", file->data);
    printf("Original size: %lu\n", file->dataSize);
    printf("Compressed size: %lu\n", file->compressedDataSize);
    printf("Extra: ");
    for (uint i = 0; i < file->extraSize; ++i) {

	printf("%x ", file->extra[i]);
    }

    printf("\n\n");
#endif
}
