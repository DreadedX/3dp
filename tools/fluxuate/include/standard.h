#ifndef STANDARD_H
#define STANDARD_H

// This header file gets precompiled and includes all neccesary header files. This file should be include in each source file.

// Standard library
#include <iostream>
#include <fstream>
#include <cstring>
#include <dirent.h>
#include <assert.h>
#include <algorithm>
#include <ctime>
#include <sys/stat.h>
#include <experimental/filesystem>

/** @todo This only works on linux */
#if WIN32
	#include <windows.h>
#else
	#include <dlfcn.h>
#endif

#include "xxhash.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"

#include "cmdline.h"

// Needed to be able to store assets in the same format as flare
// #include "flare/assets.h"

#include "plugin.h"

#endif

