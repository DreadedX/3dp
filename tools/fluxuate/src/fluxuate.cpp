#include "standard.h"

void getDir(std::string dir, std::vector<std::string> &files);
void getFile(std::string basePath, std::string fileName, flux::FileWrite *file);
bool hasChanged(std::string filePath);

void writeCache(flux::FileWrite *file, std::string fileName);
void readCache(flux::FileWrite *file, std::string fileName);

jsoncons::json cache;
jsoncons::json cacheOld;

int main(int argc, char* argv[]) {

	static_assert(sizeof(glm::vec3) == 12, "ERROR");

	clock_t t = clock();

	std::string dir;
	if (argc == 2) {

		dir = argv[1];
	} else {

		print::e("Please specify an asset folder!\n");
		exit(-1);
	}
	std::vector<std::string> files = std::vector<std::string>();

	getDir(dir, files);
	uint count = files.size();

	flux::FileWrite *fluxFiles = new flux::FileWrite[count];

	// NOTE: Uncomment to enable caching
	cacheOld = jsoncons::json::parse_file("cache/cache.json");

	for (uint i = 0; i < count; ++i) {

		getFile(dir, files[i].c_str(), &fluxFiles[i]);
	}

	std::ofstream os("cache/cache.json");
	os << cache;

	ulong totalSize = 0;
	ulong bytes_written = 0;
	// TODO: Determine the output file name based on the directory
	FILE *file = fopen("base.flx", "wb");

	ulong dataLocation = 4 + sizeof(uint);
	for (uint i = 0; i < count; i++) {

		dataLocation += sizeof(byte);
		dataLocation += fluxFiles[i].nameSize;
		dataLocation += sizeof(uint);
		dataLocation += fluxFiles[i].extraSize;
		dataLocation += sizeof(uint);
		dataLocation += sizeof(uint);
		dataLocation += sizeof(ulong);
	}
	totalSize += dataLocation;
	for (uint i = 0; i < count; i++) {

		fluxFiles[i].dataLocation = dataLocation;
		dataLocation += fluxFiles[i].compressedDataSize;
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
		bytes_written += fwrite(&fluxFiles[i].dataLocation, sizeof(byte), sizeof(ulong), file);
	}
	for (uint i = 0; i < count; i++) {

		bytes_written += fwrite(fluxFiles[i].data, sizeof(byte), fluxFiles[i].compressedDataSize, file);

		totalSize += fluxFiles[i].compressedDataSize;
	}
	fclose(file);
	assert(bytes_written == totalSize);
	print::m("Total data size: %llu\n", totalSize);

	// Preventing memory leaks
	for (uint i = 0; i < count; i++) {

		delete[] fluxFiles[i].data;
		fluxFiles[i].data = nullptr;
		delete[] fluxFiles[i].extra;
		fluxFiles[i].extra = nullptr;
	}
	delete[] fluxFiles;

	double delta = difftime(clock(), t);
	print::d("Packing took %f seconds", delta/CLOCKS_PER_SEC);
}

void getDir(std::string dir, std::vector<std::string> &files) {

	DIR *dp;
	struct dirent *dirp;

	if ((dp = opendir(dir.c_str())) == NULL) {

		print::e("Error (%i) while opening %s\n", errno, dir.c_str());
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

	// TODO: Check if file acctualy has changed, if not use cache

	// TODO: Make this based on the actual filePath
	std::string filePath = basePath + "/" + fileName;

	if (hasChanged(filePath)) {

		print::m("Loading from cache: %s (%s)", assetName.c_str(), extension.c_str());

		readCache(file, fileName);
		// NOTE: For some weird reason, this prints weird stuff, but it writes the correct stuff to the file...
	} else {

		print::m("Packing: %s (%s)", assetName.c_str(), extension.c_str());

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

				print::e("Invalid plugin: ", error);
				print::e(error);
				exit(-1);
			}
#endif

			/** @todo This function has a kind of weird name */
			Plugin plugin = getPlugin();

			print::m("Plugin name: %s", plugin.name);
			print::m("Plugin desc: %s", plugin.description);

			plugin.load(filePath.c_str(), file);

		} else {

			print::m("No plugin found for file format: %s", extension.c_str());
#if WIN32
			print::d("%li", error);
#else
			print::d("%s", error);
#endif

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
		// Compress the data
		uint uncompressedDataSize = file->dataSize;
		byte *uncompressedData = file->data;
		// Temporary size
		uLongf compressedDataSize = file->dataSize + 1024;
		byte *compressedData = new byte[compressedDataSize];

		int result = compress(compressedData, &compressedDataSize, uncompressedData, uncompressedDataSize);
		if (result != Z_OK) {
			print::e("Compression of '%s' failed (%i)\n", file->name.c_str(), result);
			exit(-1);
		}

		delete[] file->data;

		file->compressedDataSize = compressedDataSize;
		file->data = compressedData;

		// TODO: Write all data to cache file
		writeCache(file, fileName);
	}

#if 0
	print::d("- - - - ASSET - - - -");
	print::d("Name: %s", file->name.c_str());
	print::d("Data adress: %p", file->data);
	print::d("Original size: %i", file->dataSize);
	print::d("Compressed size: %i", file->compressedDataSize);
	print::d("Extra: ");
	for (uint i = 0; i < file->extraSize; ++i) {

		print::d("%x ", file->extra[i]);
	}
#endif
}

/** @todo This needs to be in a seperate file 
	@todo Clean up this function */
bool hasChanged(std::string filePath) {

	struct stat attr;
	stat(filePath.c_str(), &attr);

	// The file has not been modified since...
	if (cacheOld.has_member(filePath) && cacheOld.get(filePath).get("modified").as<time_t>() == attr.st_mtime) {

		print::d("File has not changed! (date)");

		jsoncons::json file;
		file.set("hash", cacheOld.get(filePath).get("hash").as<std::string>());
		file.set("modified", attr.st_mtime);

		cache.set(filePath, file);

		return true;
	}

	MD5 md5;

	std::string hash(md5.digestFile(strdup(filePath.c_str())));

	jsoncons::json file;
	file.set("hash", hash);
	file.set("modified", attr.st_mtime);

	cache.set(filePath, file);

	if (cacheOld.has_member(filePath)) {

		std::string hashOld = cacheOld.get(filePath).get("hash").as<std::string>();

		if (hashOld == hash) {

			print::d("File has not changed! (hash)");

			return true;
		} else {

			print::d("OLD: %s NEW: %s", hashOld.c_str(), hash.c_str());
			print::d("File has changed!");

			return false;
		}
	} else {

		print::d("File has changed!");

		return false;
	}
}

void writeCache(flux::FileWrite *file, std::string fileName) {

	std::ofstream os;
	os.open("cache/" + fileName + ".flxcache", std::ios::out | std::ios::binary);

	os.write(reinterpret_cast<const char*>(&file->nameSize), sizeof(file->nameSize));
	os.write(file->name.c_str(), file->nameSize);
	os.write(reinterpret_cast<const char*>(&file->extraSize), sizeof(file->extraSize));
	os.write(reinterpret_cast<const char*>(file->extra), file->extraSize);
	os.write(reinterpret_cast<const char*>(&file->dataSize), sizeof(file->dataSize));
	os.write(reinterpret_cast<const char*>(&file->compressedDataSize), sizeof(file->compressedDataSize));
	// TODO: Data location needs to be calculated when packaging
	// os.write(reinterpret_cast<const char*>(&file->dataLocation), sizeof(file->dataLocation));
	os.write(reinterpret_cast<const char*>(file->data), file->compressedDataSize);
}

void readCache(flux::FileWrite *file, std::string fileName) {

	std::ifstream is;
	is.open("cache/" + fileName + ".flxcache", std::ios::out | std::ios::binary);

	is.read(reinterpret_cast<char*>(&file->nameSize), sizeof(file->nameSize));

	char *name = new char[file->nameSize];
	is.read(name, file->nameSize);
	// NOTE: Char array is correct
	file->name = std::string(name);
	delete[] name;
	
	is.read(reinterpret_cast<char*>(&file->extraSize), sizeof(file->extraSize));
	file->extra = new byte[file->extraSize];
	is.read(reinterpret_cast<char*>(file->extra), file->extraSize);

	is.read(reinterpret_cast<char*>(&file->dataSize), sizeof(file->dataSize));
	is.read(reinterpret_cast<char*>(&file->compressedDataSize), sizeof(file->compressedDataSize));
	file->data = new byte[file->compressedDataSize];
	is.read(reinterpret_cast<char*>(file->data), file->compressedDataSize);
}
