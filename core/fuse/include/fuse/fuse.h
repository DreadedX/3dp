#ifndef FUSE_H
#define FUSE_H

#include <vector>
#include <algorithm>
#include <utility>
#include <array>
#include <bitset>
#include <cassert>

#include "extra/extra.h"
#include "extra/allocator/allocator.h"

/** @brief Entity-component system */
namespace fuse {};

#ifndef MAX_COMPONENTS
	/** @brief Maximum allowed components
		@todo This should propably get bigger in the future (Maybe automatically?) */
    #define MAX_COMPONENTS 32
#endif

#include "fuse/component.h"
#include "fuse/entity.h"
#include "fuse/manager.h"

#endif

