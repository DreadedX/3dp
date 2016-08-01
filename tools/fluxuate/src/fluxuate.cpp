#include "standard.h"

void getDir(std::string dir, Array<std::string> &files, std::string prefix = "");
void getFile(std::string basePath, std::string fileName, Array<flux::FileWrite*> *files);
bool hasChanged(std::string filePath);

void writeCache(flux::FileWrite *file, std::string fileName);
void readCache(flux::FileWrite *file, std::string fileName);

jsoncons::json cache;
jsoncons::json cacheOld;

std::string packageName;

/** @todo This program contains some really unorganized and confusing code, really needs a refactor */

int main(int argc, char* argv[]) {

	cmdline::parser a;

	a.add<std::string>("pkg", 'p', "Package name", true, "");
	a.add<std::string>("dir", 'd', "Directory to package", true, "");

	a.parse_check(argc, argv);

	clock_t t = clock();

	std::string dir = a.get<std::string>("dir");
	packageName = a.get<std::string>("pkg");

	Array<std::string> files;

	getDir(dir, files);

	uint count = files.size();

	//flux::FileWrite *fluxFiles = new flux::FileWrite[count];
	Array<flux::FileWrite*> fluxFiles;

	// NOTE: Uncomment to enable caching
	cacheOld = jsoncons::json::parse_file("cache/cache.json");

	for (uint i = 0; i < count; ++i) {

		getFile(dir, files[i].c_str(), &fluxFiles);
	}

	count = fluxFiles.size();

	for (flux::FileWrite *file : fluxFiles) {

		file->nameSize = file->name.length();

		// Compress the data
		uint uncompressedDataSize = file->dataSize;
		byte *uncompressedData = file->data;
		// Temporary size
		uLongf compressedDataSize = file->dataSize + 1024;
		byte *compressedData = new byte[compressedDataSize];

		int result = compress(compressedData, &compressedDataSize, uncompressedData, uncompressedDataSize);
		if (result != Z_OK) {
			print::e("Compression of '%s' failed (%i)", file->name.c_str(), result);
			exit(-1);
		}

		delete[] file->data;

		file->compressedDataSize = compressedDataSize;
		file->data = compressedData;
	}

	std::ofstream os("cache/cache.json");
	os << cache;

	ulong totalSize = 0;
	ulong bytes_written = 0;
	// TODO: Determine the output file name based on the directory
	FILE *file = fopen((packageName + ".flx").c_str(), "w+b");

	ulong dataLocation = 4 + sizeof(uint);
	for (flux::FileWrite *fluxFile : fluxFiles) {

		dataLocation += sizeof(byte);
		dataLocation += fluxFile->nameSize;
		dataLocation += sizeof(uint);
		dataLocation += sizeof(uint);
		dataLocation += sizeof(ulong);
	}
	totalSize += dataLocation;
	for (flux::FileWrite *fluxFile : fluxFiles) {

		fluxFile->dataLocation = dataLocation;
		dataLocation += fluxFile->compressedDataSize;
	}
	/** @todo This needs to move into flux */
	bytes_written += fwrite("FLX1", sizeof(byte), 4, file);
	bytes_written += fwrite(&count, sizeof(byte), sizeof(uint), file);
	for (flux::FileWrite *fluxFile : fluxFiles) {

		bytes_written += fwrite(&fluxFile->nameSize, sizeof(byte), sizeof(byte), file);
		bytes_written += fwrite(fluxFile->name.c_str(), sizeof(byte), fluxFile->nameSize, file);
		bytes_written += fwrite(&fluxFile->dataSize, sizeof(byte), sizeof(uint), file);
		bytes_written += fwrite(&fluxFile->compressedDataSize, sizeof(byte), sizeof(uint), file);
		bytes_written += fwrite(&fluxFile->dataLocation, sizeof(byte), sizeof(ulong), file);
	}
	for (flux::FileWrite *fluxFile : fluxFiles) {

		bytes_written += fwrite(fluxFile->data, sizeof(byte), fluxFile->compressedDataSize, file);

		totalSize += fluxFile->compressedDataSize;
	}

	fseek(file, 0, SEEK_END);
	ulong fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	uint adler = adler32(0L, Z_NULL, 0);

	uint length = 1024;
	byte *buffer = nullptr;

	bool shouldContinue = true;

	while(shouldContinue) {

		ulong currentPosition = ftell(file);

		if (fileSize - currentPosition < length) {

			length = static_cast<uint>(fileSize - currentPosition);

			shouldContinue = false;
		}

		buffer = new byte[length];

		uint read = fread(buffer, sizeof(byte), length, file);
		assert(length == read);
		adler = adler32(adler, buffer, length);

		delete[] buffer;
		buffer = nullptr;
	}

	fseek(file, 0, SEEK_END);

	bytes_written += fwrite(&adler, sizeof(byte), sizeof(uint), file);
	totalSize += sizeof(uint);

	fclose(file);

	assert(bytes_written == totalSize);
	print::d("======");
	print::m("Total data size: %llu", totalSize);

	// Preventing memory leaks
	for (flux::FileWrite *fluxFile : fluxFiles) {

		delete[] fluxFile->data;
		fluxFile->data = nullptr;

		delete fluxFile;
		fluxFile = nullptr;
	}

	fluxFiles.clear();

	double delta = difftime(clock(), t);
	print::d("Packing took %f seconds", delta/CLOCKS_PER_SEC);
}

void getDir(std::string dir, Array<std::string> &files, std::string prefix) {

	DIR *dp;
	struct dirent *entry;

	if ((dp = opendir(dir.c_str())) == NULL) {

		print::e("Error (%i) while opening %s", errno, dir.c_str());
		return;
	}

	while ((entry = readdir(dp)) != NULL) {

		if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {

			if (entry->d_type == DT_DIR) {

				getDir(dir + "/" + entry->d_name, files, prefix + entry->d_name + "/");

			} else {

				files.add(std::string(prefix + entry->d_name));
			}
		}
	}

	closedir(dp);
}

void getFile(std::string basePath, std::string fileName, Array<flux::FileWrite*> *files) {

	print::d("======");

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

	// TODO: Make this based on the actual filePath
	std::string filePath = basePath + "/" + fileName;

	// TODO: This needs needs to go automatically (recursive directory)
	std::string assetName = packageName + "/" + baseName;

	// TODO: Fix caching
	// if (hasChanged(filePath)) {
	if (false) {
    //
	// 	print::m("Loading from cache: %s (%s)", assetName.c_str(), extension.c_str());
    //
	// 	readCache(file, fileName);
	// 	// NOTE: For some weird reason, this prints weird stuff, but it writes the correct stuff to the file...
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

			print::m("Using: %s", plugin.name);

			plugin.load(assetName, filePath, files);

		} else {

#if WIN32
			print::d("%li", error);
#else
			print::d("%s", error);
#endif

			flux::FileWrite *file = new flux::FileWrite;
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

		// TODO: Write all data to cache file
		// writeCache(file, fileName);

	}
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
	
	is.read(reinterpret_cast<char*>(&file->dataSize), sizeof(file->dataSize));
	is.read(reinterpret_cast<char*>(&file->compressedDataSize), sizeof(file->compressedDataSize));
	file->data = new byte[file->compressedDataSize];
	is.read(reinterpret_cast<char*>(file->data), file->compressedDataSize);
}
