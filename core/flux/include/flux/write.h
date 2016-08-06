#ifndef WRITE_H
#define WRITE_H

namespace flux {

	/** @brief FLX single file header variant for writing data, stores pointer to data that needs to be written */
	struct FileWrite : FileBase {

		/** @brief Pointer to the data that needs to be stored */
		byte *data = nullptr;
		unsigned long long hash = 0;
	};
}

#endif

