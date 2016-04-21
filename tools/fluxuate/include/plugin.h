#ifndef PLUGIN_H
#define PLUGIN_H

// This should propably have a function pointer or something
struct Plugin {

	typedef void (*loadPtr)(std::string filePath, flux::FileWrite *file);

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

