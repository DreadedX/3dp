#ifndef FLARE_H
#define FLARE_H

// This header file includes all game engine include.

#ifndef GIT_VERSION
#define GIT_VERSION "Unknown"
#endif

// #include <map>
// #include <string>
#include <random>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "jsoncons/json.hpp"

#pragma GCC diagnostic pop

#ifndef NDEBUG
    #include "imgui.h"
    #include "imgui_impl_glfw_gl3.h"
#endif

/** @brief Flare engine interface */
namespace flare{

	/** @brief Components used by the flare engine */
	namespace component {};

	struct State;
	struct ProxyAllocators;
};

#include "extra/allocator/allocator.h"
#include "extra/allocator/free_list_allocator.h"
#include "extra/allocator/proxy_allocator.h"

#ifndef NDEBUG
	#ifndef NO_LEAKAGE
		// #include "debug_new.h"
	#endif
#endif

#include "extra/extra.h"
#include "flux/flux.h"
#include "fuse/fuse.h"

#include "flare/debug.h"
#include "flare/engine.h"
#include "flare/input.h"

#include "flare/assets.h"
#include "flare/components.h"

#include "flare/passes.h"
#include "flare/render.h"

#include "flare/info.h"

#include "flare/game_state.h"
#include "flare/state.h"

#endif

