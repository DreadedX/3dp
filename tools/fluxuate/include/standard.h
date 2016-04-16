#ifndef STANDARD_H
#define STANDARD_H

// This header file gets precompiled and includes all neccesary header files. This file should be include in each source file.

// Standard library
#include <iostream>
#include <vector>
#include <cstring>
#include <dirent.h>
#include <assert.h>
#include <algorithm>

// System libraries
#include <png.h>
#include <zlib.h>

// Debug
#ifndef NDEBUG
    #include "debug_new.h"
#endif

#include "extra/extra.h"
#include "flux/flux.h"

// Fluxuate include
#include "pngl.h"

#endif

