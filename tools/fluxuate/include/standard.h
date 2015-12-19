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
#if DEBUG_MODE
    #include "leakage/debug_new.h"
#endif

// Fluxuate include
#include "flux.h"
#include "pngl.h"

#endif

