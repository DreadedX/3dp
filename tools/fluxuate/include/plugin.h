#ifndef PLUGIN_H
#define PLUGIN_H

#include <iostream>
#include <vector>

#include "extra/array.h"
#include "flux/file.h"

// This should propably have a function pointer or something
struct Plugin {

	typedef void (*loadPtr)(std::string assetName, std::string filePath, Array<flux::File*> *files);

	const char *name;
	const char *description;
	loadPtr load;
	// const char **fileTypes;

	Plugin(const char *name, const char* description, loadPtr load) {

		this->name = name;
		this->description = description;
		this->load = load;
	}
};

#endif

