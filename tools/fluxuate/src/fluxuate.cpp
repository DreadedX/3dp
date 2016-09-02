#include "standard.h"

#include "extra/print.h"
#include "flux/write.h"

void getDir(std::string dir, Array<std::string> &fileNames, std::string prefix = "");
void getFile(std::string basePath, std::string fileName, Array<flux::File*> *files, Array<flux::File*> *cached);

std::string packageName;

rapidjson::Document d;

/** @todo This program contains some really unorganized and confusing code, really needs a refactor */

unsigned long long generateHash(const char *filePath) {

		FILE *hashFile = fopen( filePath, "rb" );
		assert(hashFile != NULL);

		fseek(hashFile, 0, SEEK_END);
		size_t len = ftell(hashFile);
		fseek(hashFile, 0, SEEK_SET);

		byte *data = new byte[len];
		size_t bytes_read = fread(data, sizeof(byte), len, hashFile);
		XXH64_hash_t hash = XXH64(data, len, 0);
		delete[] data;

		// Make sure it read all the data
		assert(bytes_read == len);
		fclose(hashFile);

		// print_d("%lu %s (hash)", hash, filePath);

		return hash;
}

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
	Array<flux::File*> cached;

	// Create the cache directory if it does not exist
	if (!std::experimental::filesystem::exists("cache")) {

		std::experimental::filesystem::create_directory("cache");

		print_d("Creating cache directory");
	}

	if (!std::experimental::filesystem::exists("cache/list.json")) {

		std::ofstream list;
		list.open("cache/list.json");
		list << "{}";
		list.close();

		print_d("Creating cache list");
	}

	FILE *fp = fopen("cache/list.json", "r");
	char readBuffer[65536];
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	d.ParseStream(is);
	fclose(fp);

	for (uint i = 0; i < count; ++i) {

		getFile(dir, fileNames[i], &files, &cached);
	}

	print_m("Compressing data");
	flux::write::compress(&files);

	for (flux::File *cacheFile : cached) {

		files.add(cacheFile);
	}

	print_m("Writing data");
	uint totalSize = flux::write::write(&files, (packageName + ".flx").c_str());

	print_m("Packaging of '%s' completed successfully", packageName);
	print_m("Total data size: %llu bytes", totalSize);

	print_m("Writing cache files");

	for (flux::File *file : files) {

		size_t slashLocation = file->name.find_first_of("/");
		size_t lastSlash = file->name.find_last_of("/");

		while(slashLocation <= lastSlash) {

			std::string dir = "cache/" + file->name.substr(0, slashLocation);

			if (!std::experimental::filesystem::exists(dir)) {

				print_d("Creating dir: %s", dir);
				std::experimental::filesystem::create_directory(dir);
			}

			slashLocation = file->name.find_first_of("/", slashLocation+1);
		}

		std::string fileName = "cache/" + file->name + ".flxcache";

		FILE *cacheFile = fopen(fileName.c_str(), "w+b");

		ulong bytes_written = 0;
		bytes_written += fwrite(&file->nameSize, sizeof(byte), sizeof(byte), cacheFile);
		bytes_written += fwrite(file->name.c_str(), sizeof(byte), file->nameSize, cacheFile);
		bytes_written += fwrite(&file->dataSize, sizeof(byte), sizeof(uint), cacheFile);
		bytes_written += fwrite(&file->compressedDataSize, sizeof(byte), sizeof(uint), cacheFile);
		bytes_written += fwrite(file->data, sizeof(byte), file->compressedDataSize, cacheFile);
	}

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

	fp = fopen("cache/list.json", "w");
	char writeBuffer[65536];
	rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

	rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
	d.Accept(writer);
	fclose(fp);
}

void getDir(std::string dir, Array<std::string> &fileNames, std::string prefix) {

	DIR *dp;
	struct dirent *entry;

	if ((dp = opendir(dir.c_str())) == NULL) {

		print_e("Error (%i) while opening %s", errno, dir);
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

void processFile(std::string baseName, std::string extension, std::string filePath, Array<flux::File*> *files) {

	// TODO: This needs needs to go automatically (recursive directory)
	std::string assetName = packageName + "/" + baseName;

	print_m("Packing: %s (%s)", assetName, extension);

	/** @todo This should be in a seperate function */
	typedef Plugin (*getPluginPointer)();

#if WIN32

	static_assert("Windows support is not working");

	// std::string pluginName = "./plugin_" + extension + ".dll";
	// HINSTANCE dll = LoadLibraryA(pluginName.c_str());
	// DWORD error = GetLastError();
	// if (error == 0) {
    //
	// 	getPluginPointer getPlugin = (getPluginPointer)GetProcAddress(dll, "getPlugin");
	// 	DWORD error = GetLastError();
	// 	if (error != 0) {
    //
	// 		print::e("Invalid plugin (%li)", error);
	// 		exit(-1);
	// 	}
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

void getCachedFile(std::string cachedName, Array<flux::File*> *cached) {

	flux::File *cachedFile = new flux::File;

	FILE *fp = fopen(("cache/" + cachedName + ".flxcache").c_str(), "r+b");

	ulong bytes_read = 0;
	bytes_read += fread(&cachedFile->nameSize, sizeof(byte), sizeof(byte), fp);

	char *name = new char[cachedFile->nameSize];
	bytes_read += fread(name, sizeof(byte), cachedFile->nameSize, fp);
	cachedFile->name = std::string(name, cachedFile->nameSize);

	bytes_read += fread(&cachedFile->dataSize, sizeof(byte), sizeof(uint), fp);
	bytes_read += fread(&cachedFile->compressedDataSize, sizeof(byte), sizeof(uint), fp);

	cachedFile->data = new byte[cachedFile->compressedDataSize];
	bytes_read += fread(cachedFile->data, sizeof(byte), cachedFile->compressedDataSize, fp);

	cached->add(cachedFile);
}

void getFile(std::string basePath, std::string fileName, Array<flux::File*> *files, Array<flux::File*> *cached) {

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
	filePath = std::experimental::filesystem::canonical(filePath);

	unsigned long long hash = generateHash(filePath.c_str());

	rapidjson::Document::AllocatorType &allocator = d.GetAllocator();
	rapidjson::Value output(rapidjson::kArrayType);

	if (d.HasMember(filePath.c_str()) && d[filePath.c_str()].HasMember("hash") && d[filePath.c_str()]["hash"].GetUint64() == hash) {

		print_m("Using cache: %s", packageName + "/" + baseName);

		// The file has not changed, this means we can use the cached file
		assert(d.HasMember(filePath.c_str()));
		assert(d[filePath.c_str()].HasMember("output"));
		assert(d[filePath.c_str()]["output"].IsArray());
		
		rapidjson::Value outputList = d[filePath.c_str()]["output"].GetArray();

		for (uint i = 0; i< outputList.Size(); i++) {

			getCachedFile(outputList[i].GetString(), cached);
		}

		output = outputList;
	} else {

		size_t fileCountBefore = (*files).size();
		processFile(baseName, extension, filePath, files);
		size_t fileCountAfter = (*files).size() - fileCountBefore;

		// Dependencies on other files are still missing
		for (size_t i = 0; i < fileCountAfter; i++) {

			std::string outputName = (*files)[(*files).size()-1-i]->name;
			rapidjson::Value name(outputName.c_str(), outputName.size(), allocator);
			output.PushBack(name, allocator);
		}

		// Now we need to make sure that we write out the cache files
	}

	if (d.HasMember(filePath.c_str())) {

		d.EraseMember(d.FindMember(filePath.c_str()));
	}

	rapidjson::Value data(rapidjson::kObjectType);
	data.AddMember("hash", rapidjson::Value().SetUint64(hash), allocator);
	data.AddMember("output", output, allocator);

	rapidjson::Value index(filePath.c_str(), filePath.size(), allocator);
	d.AddMember(index, data, allocator);
}
