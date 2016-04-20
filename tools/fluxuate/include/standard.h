#ifndef STANDARD_H
#define STANDARD_H

// This header file gets precompiled and includes all neccesary header files. This file should be include in each source file.

// Standard library
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <dirent.h>
#include <assert.h>
#include <algorithm>
#include <ctime>

// System libraries
#include <png.h>
#include <zlib.h>

#include "jsoncons/json.hpp"

#include "tinyobjloader/tiny_obj_loader.h"

// Needed to be able to store assets in the same format as flare
#include "flare/flare.h"

// Debug
#ifndef NDEBUG
    #include "debug_new.h"
#endif

#include "md5.h"
#include "extra/extra.h"
#include "flux/flux.h"

// Fluxuate include
#include "pngReader.h"
#include "objReader.h"

#endif

