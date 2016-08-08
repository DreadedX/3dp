#ifndef READ_H
#define READ_H

#include "extra/allocator.h"
#include "flux/file.h"

namespace flux::read {

	#define BUFFER_SIZE 1024

	/** @brief Initialize system */
	void init(Allocator *allocator);
	/** @brief Load all FLX containers in current directory */
	void load();
	/** @brief Get pointer to FLX single file header, this file could be in any of the loaded containers */
	File *get(std::string name);
	/** @brief Close all the loaded FLX containers */
	void close();

	/** @brief Close all FLX containers and reopen them */
	void reload();
}

#endif

