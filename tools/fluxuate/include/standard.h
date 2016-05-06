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

/** @todo This only works on linux */
#if WIN32
	#include <windows.h>
#else
	#include <dlfcn.h>
#endif

// System libraries
#include <png.h>
#include <zlib.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "jsoncons/json.hpp"

#pragma GCC diagnostic pop

#include "tinyobjloader/tiny_obj_loader.h"

#include "cmdline/cmdline.h"

// Needed to be able to store assets in the same format as flare
// #include "flare/assets.h"

// Debug
#ifndef NDEBUG
    #include "debug_new.h"
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses"

#include "md5.h"

#pragma GCC diagnostic pop

#include "extra/extra.h"
#include "flux/flux.h"

#include "plugin.h"

#endif

