#include "standard.h"

#include "extra/print.h"
#include "flux/write.h"

void getDir(std::string dir, Array<std::string> &fileNames, std::string prefix = "");
void getFile(std::string basePath, std::string fileName, Array<flux::File*> *files);

std::string packageName;

/** @todo This program contains some really unorganized and confusing code, really needs a refactor */

// unsigned long long generateHash(const char *filePath) {
//
// 		FILE *hashFile = fopen( filePath, "rb" );
// 		assert(hashFile != NULL);
//
// 		fseek(hashFile, 0, SEEK_END);
// 		size_t len = ftell(hashFile);
// 		fseek(hashFile, 0, SEEK_SET);
//
// 		byte *data = new byte[len];
// 		size_t bytes_read = fread(data, sizeof(byte), len, hashFile);
// 		XXH64_hash_t hash = XXH64(data, len, 0);
// 		delete[] data;
//
// 		// Make sure it read all the data
// 		assert(bytes_read == len);
// 		fclose(hashFile);
//
// 		char actualpath[PATH_MAX+1];
// 		print::d("%lu %s (hash)", hash, realpath(filePath, actualpath));
//
// 		return hash;
// }

int main(int argc, char* argv[]) {

	cmdline::parser a;

	a.add<std::string>("pkg", 'p', "Package name", true, "");
	a.add<std::string>("dir", 'd', "Directory to package", true, "");

	a.parse_check(argc, argv);

	clock_t t = clock();

	std::string dir = a.get<std::string>("dir");
	packageName = a.get<std::string>("pkg");

	Array<std::string> fileNames;

	getDir(dir, fileNames);

	uint count = fileNames.size();

	Array<flux::File*> files;

	for (uint i = 0; i < count; ++i) {

		getFile(dir, fileNames[i].c_str(), &files);
	}

	print_m("Compressing data");
	flux::write::compress(&files);

	print_m("Writing data");
	uint totalSize = flux::write::write(&files, (packageName + ".flx").c_str());

	print_m("Packaging of '%s' completed successfully", packageName.c_str());
	print_m("Total data size: %llu", totalSize);

	// Preventing memory leaks
	for (flux::File *file : files) {

		delete[] file->data;
		file->data = nullptr;

		delete file;
		file = nullptr;
	}

	files.clear();

	double delta = difftime(clock(), t);
	print_d("Packing took %f seconds", delta/CLOCKS_PER_SEC);
}

void getDir(std::string dir, Array<std::string> &fileNames, std::string prefix) {

	DIR *dp;
	struct dirent *entry;

	if ((dp = opendir(dir.c_str())) == NULL) {

		print_e("Error (%i) while opening %s", errno, dir.c_str());
		return;
	}

	while ((entry = readdir(dp)) != NULL) {

		if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {

			if (entry->d_type == DT_DIR) {

				getDir(dir + "/" + entry->d_name, fileNames, prefix + entry->d_name + "/");

			} else {

				fileNames.add(std::string(prefix + entry->d_name));
			}
		}
	}

	closedir(dp);
}

void getFile(std::string basePath, std::string fileName, Array<flux::File*> *files) {

	std::string extension;
	std::string baseName;
	int length = fileName.length();
	int lastPoint = fileName.find_last_of(".");

	if (lastPoint != 0) {

		extension = fileName.substr(lastPoint+1, length);
		baseName = fileName.substr(0, lastPoint);
	} else {

		extension = "";
		baseName = fileName;
	}

	// TODO: Make this based on the actual filePath
	std::string filePath = basePath + "/" + fileName;

	// TODO: This needs needs to go automatically (recursive directory)
	std::string assetName = packageName + "/" + baseName;

	print_m("Packing: %s (%s)", assetName.c_str(), extension.c_str());

	/** @todo This should be in a seperate function */
	typedef Plugin (*getPluginPointer)();

#if WIN32
	std::string pluginName = "./plugin_" + extension + ".dll";
	HINSTANCE dll = LoadLibraryA(pluginName.c_str());
	DWORD error = GetLastError();
	if (error == 0) {

		getPluginPointer getPlugin = (getPluginPointer)GetProcAddress(dll, "getPlugin");
		DWORD error = GetLastError();
		if (error != 0) {

			print::e("Invalid plugin (%li)", error);
			exit(-1);
		}
#else
	std::string pluginName = "./libplugin_" + extension + ".so";
	void *handle = dlopen(pluginName.c_str(), RTLD_NOW);
	char *error = dlerror();
	if (error == nullptr) {

		getPluginPointer getPlugin = (getPluginPointer)dlsym(handle, "getPlugin");
		error = dlerror();
		if (error != nullptr) {

			print_e("Invalid plugin: ", error);
			print_e(error);
			exit(-1);
		}
#endif

		/** @todo This function has a kind of weird name */
		Plugin plugin = getPlugin();

		print_m("Using: %s", plugin.name);

		plugin.load(assetName, filePath, files);

	} else {

		flux::File *file = new flux::File;
		files->add(file);

		file->name = assetName;

		if (extension == "vertex" || extension == "fragment") {

			std::string assetNameSpecial = assetName + "_" + extension;
			file->nameSize = assetNameSpecial.size();
			file->name = assetNameSpecial;
		}

		if (extension == "mtl") {

			std::string assetNameSpecial = assetName + "_material";
			file->nameSize = assetNameSpecial.size();
			file->name = assetNameSpecial;
		}

		FILE *sourceFile = fopen(filePath.c_str(), "rb");
		assert(sourceFile != NULL);

		fseek(sourceFile, 0, SEEK_END);
		file->dataSize = ftell(sourceFile);
		fseek(sourceFile, 0, SEEK_SET);

		file->data = new byte[file->dataSize];
		ulong bytes_read = fread(file->data, sizeof(byte), file->dataSize, sourceFile);

		// Make sure it read all the data
		assert(bytes_read == file->dataSize);
		fclose(sourceFile);
	}
}
