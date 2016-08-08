#ifndef FILE_H
#define FILE_H

#include <string>
#include "extra/typedef.h"

namespace flux {

	struct Container;

	/** @brief FLX single file header */
	struct File {

		/** @brief Length of the file name */
		byte nameSize = 0;
		/** @brief File name
			@todo This could probably be a char* */
		std::string name = "";

		/** @brief Uncompressed data size */
		uint dataSize = 0;
		/** @brief Compressed data size */
		uint compressedDataSize = 0;
		/** @brief Location of compressed data in container */
		ulong dataLocation = 0;

		Container *parent = nullptr;
		byte *data = nullptr;

		byte *load();
	};

	/** @brief FLX container data */
	struct Container {

		/** @brief Name of the FLX container */
		std::string name = "";
		/** @brief Handle to the container file */
		FILE *fileHandle = nullptr;
		/** @brief Amount of files that are stored in the container */
		uint indexSize = 0;
		/** @brief Array containing all the FLX single file headers */
		File *index = nullptr;

		/** @brief Stores if the file is valid */
		bool valid = false;

		/** @brief Initialize FLX container and load all the FLX single file headers
			@param name - Name of the container to load */
		void load(std::string name);

		/** @brief Close the filehandle and remove all FLX single file headers */
		void close();
	};
}

#endif

