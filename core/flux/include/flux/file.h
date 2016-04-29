#ifndef LOAD_H
#define LOAD_H

namespace flux {

	/** @brief FLX single file header */
	struct FileBase {

		/** @brief Length of the file name */
		byte nameSize = 0;
		/** @brief File name
			@todo This could probably be a char* */
		std::string name = "";

		/** @brief Size of the extra bytes */
		uint extraSize = 0;
		/** @brief Array containing extra bytes */
		byte *extra = nullptr;

		/** @brief Uncompressed data size */
		uint dataSize = 0;
		/** @brief Compressed data size */
		uint compressedDataSize = 0;
		/** @brief Location of compressed data in container */
		ulong dataLocation = 0;
	};

}

#endif

