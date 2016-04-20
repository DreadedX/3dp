#include "standard.h"

void getDir(std::string dir, std::vector<std::string> &files);
void getFile(std::string basePath, std::string fileName, flux::FileWrite *file);
bool compareHash(std::string filePath);

void writeCache(flux::FileWrite *file, std::string fileName);
void readCache(flux::FileWrite *file, std::string fileName);

jsoncons::json hashes;
jsoncons::json hashesOld;

int main(int argc, char* argv[]) {

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
	hashesOld = jsoncons::json::parse_file("cache/cache.json");

	for (uint i = 0; i < count; ++i) {

		getFile(dir, files[i].c_str(), &fluxFiles[i]);
	}

	std::ofstream os("cache/cache.json");
	os << hashes;

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
		bytes_written += fwrite(&fluxFiles[i].dataLocation, sizeof(byte), sizeof(llu), file);
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

	if (compareHash(filePath)) {

		print::m("Loading from cache: %s (%s)", assetName.c_str(), extension.c_str());

		readCache(file, fileName);
		// NOTE: For some weird reason, this prints weird stuff, but it writes the correct stuff to the file...
	} else {

		print::m("Packing: %s (%s)", assetName.c_str(), extension.c_str());

		// Images are a special case
		// Now we don't need have to use libpng in the engine
		if (extension == "png") {

			png::Data data = png::read(filePath.c_str());

			file->extraSize = sizeof(int) * 2 + sizeof(byte);
			file->extra = new byte[file->extraSize];

			for (uint i = 0; i < sizeof(int); ++i) {

				file->extra[i] = data.width >> (i*8);
			}
			for (uint i = 0; i < sizeof(int); ++i) {

				file->extra[i+sizeof(int)] = data.height >> (i*8);
			}
			file->extra[sizeof(int)*2] = data.bytesPerPixel;

			// file->data = data.pixels;
			file->dataSize = data.size;
			file->data = new byte[file->dataSize];
			for (int y = 0; y < data.height; y++) {
				for (int x = 0; x < data.width*data.bytesPerPixel; x++) {

					// file->data[x + y * (data.width*data.bytesPerPixel)] = data.pixels[x + (data.height-y-1) * (data.width*data.bytesPerPixel)];
					file->data[x + y * (data.width*data.bytesPerPixel)] = data.pixels[x + y * (data.width*data.bytesPerPixel)];
				}
			}
		} else if(extension == "obj") {

			// TESTING
			flare::asset::Model model = obj::read(filePath.c_str());

			// TODO: Should this really be stored in extra
			unsigned long meshCount = model.meshes.size();
			file->extraSize = sizeof(unsigned long) + meshCount * (sizeof(unsigned long) + sizeof(unsigned long));
			file->extra = new byte[file->extraSize];

			for (uint i = 0; i < sizeof(unsigned long); ++i) {

				file->extra[i] = meshCount >> (i*8);
			}

			uint offset = sizeof(unsigned long);

			print::d("Mesh count: %i", meshCount);
			for (unsigned long j = 0; j < meshCount; ++j) {

				unsigned long vertexCount = model.meshes[j]->vertices.size();
				unsigned long indexCount = model.meshes[j]->indices.size();

				print::d("Vertex count: %i", vertexCount);
				print::d("Index count: %i", indexCount);

				file->dataSize += vertexCount * sizeof(flare::asset::model::Vertex);
				file->dataSize += indexCount * sizeof(GLuint);

				for (uint i = 0; i < sizeof(unsigned long); ++i) {

					file->extra[i + offset] = vertexCount >> (i*8);
				}
				for (uint i = 0; i < sizeof(unsigned long); ++i) {

					file->extra[i + offset + sizeof(unsigned long)] = indexCount >> (i*8);
				}

				offset += 2*sizeof(unsigned long);
			}

			offset = 0;

			file->data = new byte[file->dataSize];

			for (uint i = 0; i < file->dataSize; ++i) {

				file->data[i] = 0x0;
			}

			for (unsigned long j = 0; j < meshCount; ++j) {

				unsigned long vertexCount = model.meshes[j]->vertices.size();
				unsigned long indexCount = model.meshes[j]->indices.size();

				for (uint i = 0; i < vertexCount; ++i) {

					glm::vec3 position = model.meshes[j]->vertices[i].position;
					glm::vec3 normal = model.meshes[j]->vertices[i].normal;
					glm::vec2 texCoords = model.meshes[j]->vertices[i].texCoords;

					// TODO: This is system depended (byte order)
					// Write position
					byte *tempData = new byte[sizeof(glm::vec3)];
					memcpy(tempData, &position, sizeof(glm::vec3));

					for (uint n = 0; n < sizeof(glm::vec3); ++n) {

						file->data[n + offset] = tempData[n];
					}
					offset += sizeof(glm::vec3);
					delete[] tempData;
					
					// Write normal
					tempData = new byte[sizeof(glm::vec3)];
					memcpy(tempData, &normal, sizeof(glm::vec3));

					for (uint n = 0; n < sizeof(glm::vec3); ++n) {

						file->data[n + offset] = tempData[n];
					}
					offset += sizeof(glm::vec3);
					delete[] tempData;
					
					// Write texCoords
					tempData = new byte[sizeof(glm::vec2)];
					memcpy(tempData, &texCoords, sizeof(glm::vec2));

					for (uint n = 0; n < sizeof(glm::vec2); ++n) {

						file->data[n + offset] = tempData[n];
					}
					offset += sizeof(glm::vec2);
					delete[] tempData;
				}
				for (uint i = 0; i < indexCount; ++i) {

					GLuint index = model.meshes[j]->indices[i];

					// Write index
					for (uint n = 0; n < sizeof(GLuint); ++n) {

						file->data[n + offset] = index >> (n*8);
					}

					offset += sizeof(GLuint);
				}
			}

			// print::d("OBJ hex:");
            //
			// for (uint i = 0; i < file->dataSize; ++i) {
            //
			// 	if (i % 32 == 0 && i != 0) {
			// 		printf("\n");
			// 	}
            //
			// 	printf("0x%02X ", file->data[i]);
			// }
            //
			// printf("\n");

			assert(offset == file->dataSize);

		} else {

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

bool compareHash(std::string filePath) {

	MD5 md5;

	std::string hash(md5.digestFile(strdup(filePath.c_str())));
	std::string hashOld = hashesOld.get(filePath).as<std::string>();
	hashes.set(filePath, std::string(hash));

	print::d("OLD: %s NEW: %s", hashOld.c_str(), hash.c_str());

	if (hash == hashOld) {

		print::d("File has not changed!");

		return true;
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
