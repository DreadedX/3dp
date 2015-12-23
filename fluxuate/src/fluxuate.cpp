#include "standard.h"

void getDir(std::string dir, std::vector<std::string> &files);
void getFile(std::string basePath, std::string fileName, flux::File *file);

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
    unsigned int count = files.size();

    flux::File *fluxFiles = new flux::File[count];

    for (unsigned int i = 0; i < count; ++i) {

	getFile(dir, files[i].c_str(), &fluxFiles[i]);
    }

    long unsigned int totalSize = 0;
    long unsigned int bytes_written = 0;
    // TODO: Determine the output file name based on the directory
    FILE *file = fopen("base.flx", "wb");

    long unsigned int dataLocation = 4 + sizeof(unsigned int);
    for (unsigned int i = 0; i < count; i++) {

	dataLocation += sizeof(unsigned char);
	dataLocation += fluxFiles[i].nameSize;
	dataLocation += sizeof(unsigned int);
	dataLocation += fluxFiles[i].extraSize;
	dataLocation += sizeof(long unsigned int);
	dataLocation += sizeof(long unsigned int);
	dataLocation += sizeof(long unsigned int);
    }
    totalSize += dataLocation;
    for (unsigned int i = 0; i < count; i++) {

	fluxFiles[i].dataLocation = dataLocation;
	dataLocation += fluxFiles[i].dataSize;
    }
    bytes_written += fwrite("FLX0", sizeof(unsigned char), 4, file);
    bytes_written += fwrite(&count, sizeof(unsigned char), sizeof(unsigned int), file);
    for (unsigned int i = 0; i < count; i++) {

	bytes_written += fwrite(&fluxFiles[i].nameSize, sizeof(unsigned char), sizeof(unsigned char), file);
	bytes_written += fwrite(fluxFiles[i].name.c_str(), sizeof(unsigned char), fluxFiles[i].nameSize, file);
	bytes_written += fwrite(&fluxFiles[i].extraSize, sizeof(unsigned char), sizeof(unsigned int), file);
	bytes_written += fwrite(fluxFiles[i].extra, sizeof(unsigned char), fluxFiles[i].extraSize, file);
	bytes_written += fwrite(&fluxFiles[i].dataSize, sizeof(unsigned char), sizeof(long unsigned int), file);
	bytes_written += fwrite(&fluxFiles[i].compressedDataSize, sizeof(unsigned char), sizeof(long unsigned int), file);
	bytes_written += fwrite(&fluxFiles[i].dataLocation, sizeof(unsigned char), sizeof(long unsigned int), file);
    }
    for (unsigned int i = 0; i < count; i++) {

	bytes_written += fwrite(fluxFiles[i].data, sizeof(unsigned char), fluxFiles[i].dataSize, file);

	totalSize += fluxFiles[i].dataSize;
    }
    fclose(file);
    assert(bytes_written == totalSize);
    printf("Total data size: %lu\n", totalSize);

    // Preventing memory leaks
    for (unsigned int i = 0; i < count; i++) {

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

void getFile(std::string basePath, std::string fileName, flux::File *file) {

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

	file->extraSize = sizeof(int) * 2 + sizeof(unsigned char);
	file->extra = new unsigned char[file->extraSize];

	for (unsigned int i = 0; i < sizeof(int); ++i) {

	    file->extra[i] = data.width >> (i*8);
	}
	for (unsigned int i = sizeof(int); i < sizeof(int)*2; ++i) {

	    file->extra[i] = data.height >> (i*8);
	}
	file->extra[sizeof(int)*2] = data.bytesPerPixel;

	// file->data = data.pixels;
	file->dataSize = data.size;
	file->data = new unsigned char[file->dataSize];
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

	file->data = new unsigned char[file->dataSize];
	long unsigned int bytes_read = fread(file->data, sizeof(unsigned char), file->dataSize, sourceFile);

	// Make sure it read all the data
	assert(bytes_read == file->dataSize);
	fclose(sourceFile);
    }

    // Compress the data
    long unsigned int uncompressedDataSize = file->dataSize;
    unsigned char *uncompressedData = file->data;
    // Temporary size
    long unsigned int compressedDataSize = file->dataSize + 1024;
    unsigned char *compressedData = new unsigned char[compressedDataSize];

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
    for (unsigned int i = 0; i < file->extraSize; ++i) {

	printf("%x ", file->extra[i]);
    }

    printf("\n\n");
#endif
}
