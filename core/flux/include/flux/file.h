#ifndef LOAD_H
#define LOAD_H

namespace flux {

	/** @brief FLX single file header */
	struct FileBase {

		/** @brief Length of the file name */
		unsigned char nameSize = 0;
		/** @brief File name
			@todo This could probably be a char* */
		std::string name = "";

		/** @brief Size of the extra bytes */
		unsigned int extraSize = 0;
		/** @brief Array containing extra bytes */
		unsigned char *extra = nullptr;

		/** @brief Uncompressed data size */
		uint dataSize = 0;
		/** @brief Compressed data size */
		uint compressedDataSize = 0;
		/** @brief Location of compressed data in container */
		llu dataLocation = 0;
	};

}

#endif

