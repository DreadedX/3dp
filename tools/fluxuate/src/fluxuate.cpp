#include "standard.h"

void getDir(std::string dir, std::vector<std::string> &files);
void processFile(std::string fileName);

int main() {

    std::string dir = "./assets";
    std::vector<std::string> files = std::vector<std::string>();

    getDir(dir, files);
    
    for (unsigned int i = 0; i < files.size(); ++i) {

	processFile(files[i].c_str());
    }
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

void processFile(std::string fileName) {

    std::string extension;
    std::string baseName;
    int length = fileName.length();
    int lastPoint = fileName.find_last_of(".");

    flux::File file;

    if (lastPoint != 0) {

	extension = fileName.substr(lastPoint+1, length);
	baseName = fileName.substr(0, lastPoint);
    } else {

	extension = "";
	baseName = fileName;
    }

    // TODO: This needs needs to go automatically (recursive directory)
    std::string assetName = "base/image/" + baseName;
    file.nameSize = assetName.size();
    file.name = assetName.c_str();

    // TODO: Make this based on the actual filePath
    std::string filePath = "./assets/" + fileName;

    if (extension == "png") {

	png::Data data = png::read(filePath.c_str());

	file.extraSize = sizeof(int) * 2 + sizeof(unsigned char);
	file.extra = new unsigned char[file.extraSize];

	for (unsigned int i = 0; i < sizeof(int); ++i) {

	    file.extra[i] = data.width >> ((sizeof(int)-i-1)*8);
	}
	for (unsigned int i = sizeof(int); i < sizeof(int)*2; ++i) {

	    file.extra[i] = data.height >> ((sizeof(int)-i-1)*8);
	}
	file.extra[sizeof(int)*2] = data.bytesPerPixel;

	file.data = data.pixels;
	file.dataSize = data.size;

    } else {

	// Unsupported file
	return;
    }

    printf("- - - NEW ASSET - - -\n");
    printf("Name: %s\n", file.name);
    printf("Size: %i\n", file.dataSize);
    printf("Extra: ");
    for (int i = 0; i < file.extraSize; ++i) {

	printf("%x ", file.extra[i]);
    }

    printf("\n\n");

}
