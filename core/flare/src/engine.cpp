#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifndef NDEBUG
	#include "imgui.h"
	#include "imgui_impl_glfw_gl3.h"
#endif

#include "flux/read.h"
#include "flare/engine.h"
#include "flare/input.h"
#include "flare/info.h"
#include "flare/debug.h"

/** @brief Pointer the settings that are currently used by the engine
  @todo Add a way to set this externally */
flare::State *state = nullptr;

const size_t RESERVED_MEMORY = 1000l * 1000l * 200l; /* 130 MB */

void *arenaStart;

/** @todo This should be moved to a seperate file */
void errorCallbackGLFW(int error, const char *description) {

	print_e("GLFW error (%i): %s", error, description);
}

void flare::init() {

	// Reserve memory
	arenaStart = malloc(RESERVED_MEMORY + sizeof(FreeListAllocator));

	// Put main allocator in reserved memory
	FreeListAllocator *mainAllocator = new (arenaStart) FreeListAllocator(RESERVED_MEMORY, pointer_math::add(arenaStart, sizeof(FreeListAllocator)), "Main");

	// Create state object
	state = allocator::make_new<State>(*mainAllocator);

	// Store pointer to main allocator in state
	getState()->mainAllocator = mainAllocator;

	// Create proxy allocators
	getState()->proxyAllocators.flux = allocator::make_new_proxy(*getState()->mainAllocator, "Flux");
	getState()->proxyAllocators.fuse = allocator::make_new_proxy(*getState()->mainAllocator, "Fuse");
	getState()->proxyAllocators.asset = allocator::make_new_proxy(*getState()->mainAllocator, "Asset");
	getState()->proxyAllocators.model = allocator::make_new_proxy(*getState()->mainAllocator, "Model");

	info::print();

	// Initialize window
	/** @todo Part of this should propably move into the renderer */

	State::Settings *settings = &getState()->settings;

	assert(settings != nullptr);

	print_d("The current resolution is %i by %i", settings->resolution.x, settings->resolution.y);

	if (!glfwInit()) {

		print_e("Failed to initialize glfw!");
		exit(-1);
	}

	glfwSetErrorCallback(errorCallbackGLFW);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// glfwWindowHint(GLFW_SAMPLES, 4);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// NOTE: First nullptr to glfwGetPrimaryMonitor for fullscreen
	getState()->window = glfwCreateWindow(settings->resolution.x, settings->resolution.y, settings->name, nullptr, nullptr);

	if (getState()->window == nullptr) {

		print_e("Failed to initialize window");
		exit(-1);
	}

	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	/** @bug This currently needs to add the width of my left monitor the get it center on my center monitor */
	glfwSetWindowPos(getState()->window, (mode->width - settings->resolution.x) / 2 + 1366, (mode->height - settings->resolution.y) / 2);

	glfwSetKeyCallback(getState()->window, input::_keyCallback);

	glfwMakeContextCurrent(getState()->window);

	glewExperimental = GL_TRUE;
	GLenum glewStatus = glewInit();
	if (glewStatus != GLEW_OK) {

		print_e("Failed to initialize glew: %s", glewGetErrorString(glewStatus));
		exit(-1);
	}

#ifndef NDEBUG
	/** @todo Create own ImGui compatible callbacks
		@bug This sets things like swap, that then need to be disabled again */
	ImGui_ImplGlfwGL3_Init(getState()->window, false);
#endif

	glfwSwapInterval(settings->swap);

	glGetError();

	// Load all asset files
	flux::read::init(getState()->proxyAllocators.flux);
	flux::read::load();

	getState()->mainState = allocator::make_new<MainState>(*getState()->mainAllocator);

	// This should all be in the state constructor or init function
	getState()->mainState->manager = allocator::make_new<fuse::Manager>(*getState()->proxyAllocators.fuse);

	// Initialize other systems
	getState()->mainState->manager->init(getState()->proxyAllocators.fuse);

	asset::init(getState()->proxyAllocators.asset);

	render::init();

	print_d("Engine initialized");
}

bool flare::isRunning() {

	return !glfwWindowShouldClose(getState()->window);
}

void flare::update() {

	DEBUG_TIME_INIT;

	// Calculate the deltaTime
	static float lastFrame = 0;
	float currentFrame = glfwGetTime();
	flare::getState()->render.deltaTime = (currentFrame - lastFrame);
	lastFrame = currentFrame;

	DEBUG_TIME( {
		glfwPollEvents();
		flare::input::update();
	}, "input" );

	// NOTE: Debug keybindings
	DEBUG_TIME( {
		if (input::keyCheck(GLFW_KEY_ESCAPE)) {

			glfwSetWindowShouldClose(getState()->window, GL_TRUE);
		}

		if (input::keyCheck(GLFW_KEY_F5)) {

			print_d("Reloading assets");
			asset::reload();
			input::keySet(GLFW_KEY_F5, false);
		}

	}, "debug keys" );

	// Run manager logic
	/** @todo The stats on this need to be more in-depth */
	DEBUG_TIME( getState()->mainState->update(), "update" );
	// Run renderer logic
	/** @todo The stats on this need to be more in-depth */
	DEBUG_TIME( getState()->mainState->draw(), "render" );

	// This is for the debug interface
#ifndef NDEBUG
		DEBUG_TIME( debug::debug(), "debug interface" );
#endif

	DEBUG_TIME( glfwSwapBuffers(getState()->window), "swap buffers" );

	char array[10];
	snprintf(array, sizeof(array), "%f", getState()->render.deltaTime * 1000);
	glfwSetWindowTitle(getState()->window, array);
}

void flare::terminate(int errorCode) {

	// Kill and remove all remaining entities
	getState()->mainState->manager->killAll();

	// Close all files
	flux::read::close();

	glfwTerminate();

	// Remove render passes
	for(render::passes::Pass *pass : getState()->mainState->renderPasses) {

		delete pass;
	}

	// Close all open asset files
	asset::close();

	print_d("The engine is now exiting");

	allocator::make_delete(*getState()->proxyAllocators.fuse, getState()->mainState->manager);

	// Remove all proxy allocators
	allocator::make_delete_proxy(*getState()->proxyAllocators.flux, *getState()->mainAllocator);
	getState()->proxyAllocators.flux = nullptr;
	allocator::make_delete_proxy(*getState()->proxyAllocators.fuse, *getState()->mainAllocator);
	getState()->proxyAllocators.fuse = nullptr;
	allocator::make_delete_proxy(*getState()->proxyAllocators.asset, *getState()->mainAllocator);
	getState()->proxyAllocators.asset = nullptr;
	allocator::make_delete_proxy(*getState()->proxyAllocators.model, *getState()->mainAllocator);
	getState()->proxyAllocators.model = nullptr;

	allocator::make_delete(*getState()->mainAllocator, *getState()->mainState);

	// Create a temporary pointer to the main allocator
	FreeListAllocator *mainAllocator = getState()->mainAllocator;

	// Delete the state object
	allocator::make_delete(*mainAllocator, *getState());

	// Call the destructor of the main allocator
	allocator::make_delete(*mainAllocator, mainAllocator);

	// Free the reserved memory
	free(arenaStart);
	arenaStart = nullptr;

	exit(errorCode);
}

flare::State *flare::getState() {

	return state;
}
