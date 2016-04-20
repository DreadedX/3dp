#ifndef FLARE_H
#define FLARE_H

// This header file includes all game engine include.

#ifndef GIT_VERSION
#define GIT_VERSION "Unknown"
#endif

#include <map>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "jsoncons/json.hpp"

#include "tinyobjloader/tiny_obj_loader.h"

#ifndef NDEBUG
    #include "imgui.h"
    #include "imgui_impl_glfw_gl3.h"
#endif

/** @brief Flare engine interface */
namespace flare{

	/** @brief Components used by the flare engine */
	namespace component {};
};

#include "extra/extra.h"
#include "flux/flux.h"
#include "fuse/fuse.h"

#ifndef NDEBUG
    #include "debug_new.h"
    #include "flare/debug.h"
#endif

#include "flare/engine.h"
#include "flare/input.h"

#include "flare/assets.h"
#include "flare/components.h"

#include "flare/render.h"

#endif

