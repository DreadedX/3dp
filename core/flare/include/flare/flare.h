#ifndef FLARE_H
#define FLARE_H

// This header file includes all game engine include.

#include <map>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#ifndef NDEBUG
    #include "imgui.h"
    #include "imgui_impl_glfw_gl3.h"
#endif

#include "extra/extra.h"
#include "flux/flux.h"
#include "fuse/fuse.h"

#ifndef NDEBUG
    #include "debug_new.h"
    #include "flare/debug.h"
#endif

#include "flare/engine.h"
#include "flare/asset.h"
#include "flare/input.h"
#include "flare/render.h"

#include "flare/components.h"

#endif

