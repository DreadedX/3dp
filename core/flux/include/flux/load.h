#ifndef FILE_H
#define FILE_H

namespace flux {

	struct Flux;

	/** @brief Variant of FLX single file header that stores parent and allows for loading the actual file data */
	struct FileLoad : FileBase {

		/** @brief Pointer to parent container */
		Flux *parent;

		/** @brief Load de actual file data
			@param addNullTerminator - append data with a null terminator, useful for loading strings
			@returns Byte array containing all file data */
		byte *get(bool addNullTerminator = false);
	};

	/** @brief FLX container data */
	struct Flux {

		/** @brief Name of the FLX container */
		std::string name = "";
		/** @brief Handle to the container file */
		FILE *fileHandle = nullptr;
		/** @brief Amount of files that are stored in the container */
		uint indexSize = 0;
		/** @brief Array containing all the FLX single file headers */
		FileLoad *index = nullptr;

		/** @brief Stores if the file is valid */
		bool valid = false;

		/** @brief Initialize FLX container and load all the FLX single file headers
			@param name - Name of the container to load */
		void load(std::string name);

		/** @brief Close the filehandle and remove all FLX single file headers */
		void close();
	};

	/** @brief Initialize system */
	void init(Allocator *allocator);
	/** @brief Load all FLX containers in current directory */
	void load();
	/** @brief Get pointer to FLX single file header, this file could be in any of the loaded containers */
	FileLoad *get(std::string name);
	/** @brief Close all the loaded FLX containers */
	void close();

	/** @brief Close all FLX containers and reopen them */
	void reload();
}

#endif

