#include "flare/flare.h"
#include <thread>
#include <chrono>

/** @brief Pointer the settings that are currently used by the engine
  @todo Add a way to set this externally */
flare::State *state = nullptr;

const size_t RESERVED_MEMORY = 1000l * 1000l * 200l; /* 130 MB */

void *arenaStart;

/** @todo This should be moved to a seperate file */
void errorCallbackGLFW(int error, const char *description) {

	print::e("GLFW error (%i): %s", error, description);
}

void flare::init() {

	// Reserve memory
	arenaStart = malloc(RESERVED_MEMORY + sizeof(FreeListAllocator));

	// Put main allocator in reserved memory
	FreeListAllocator *tempAllocator = new (arenaStart) FreeListAllocator(RESERVED_MEMORY, pointer_math::add(arenaStart, sizeof(FreeListAllocator)));

	// Create state object
	state = allocator::make_new<State>(*tempAllocator);

	// Store pointer to main allocator in state
	getState()->mainAllocator = tempAllocator;

	// Create proxy allocators
	getState()->proxyAllocators.flux = allocator::make_new_proxy(*getState()->mainAllocator);
	getState()->proxyAllocators.fuse = allocator::make_new_proxy(*getState()->mainAllocator);
	getState()->proxyAllocators.asset = allocator::make_new_proxy(*getState()->mainAllocator);
	getState()->proxyAllocators.model = allocator::make_new_proxy(*getState()->mainAllocator);
	getState()->proxyAllocators.entities = allocator::make_new_proxy(*getState()->mainAllocator);

	info::print();

	// Initialize window
	/** @todo Part of this should propably move into the renderer */

	State::Settings *settings = &getState()->settings;

	assert(settings != nullptr);

	print::d("The current resolution is %i by %i", settings->resolution.x, settings->resolution.y);

	if (!glfwInit()) {

		print::e("Failed to initialize glfw!");
		exit(-1);
	}

	glfwSetErrorCallback(errorCallbackGLFW);

	/** @note Disabled for testing */
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// glfwWindowHint(GLFW_SAMPLES, 4);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// NOTE: First nullptr to glfwGetPrimaryMonitor for fullscreen
	getState()->window = glfwCreateWindow(settings->resolution.x, settings->resolution.y, settings->name, nullptr, nullptr);

	if (getState()->window == nullptr) {

		print::e("Failed to initialize window");
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

		print::e("Failed to initialize glew: %s", glewGetErrorString(glewStatus));
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
	flux::init(getState()->proxyAllocators.flux);
	flux::load();

	getState()->mainState = allocator::make_new<MainState>(*getState()->mainAllocator);

	// This should all be in the state constructor or init function
	getState()->mainState->manager = allocator::make_new<fuse::Manager>(*getState()->proxyAllocators.entities);

	// Initialize other systems
	getState()->mainState->manager->init(getState()->proxyAllocators.fuse);

	asset::init(getState()->proxyAllocators.asset);

	render::init();

	print::d("Engine initialized");
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

			print::d("Reloading assets");
			asset::reload();
			input::keySet(GLFW_KEY_F5, false);
		}

		if (input::keyCheck(GLFW_KEY_Z)) {
			static bool wireframe = false;

			if (wireframe) {
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
				wireframe = false;
			} else {
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
				wireframe = true;
			}
			input::keySet(GLFW_KEY_Z, false);
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
}

void flare::terminate(int errorCode) {

	// Kill and remove all remaining entities
	getState()->mainState->manager->killAll();
	// Make sure the manager actually deletes the entities
	getState()->mainState->manager->update();

	// Close all files
	flux::close();

	glfwTerminate();

	// Close all open asset files
	asset::close();

	print::d("The engine is now exiting");

	// Remove all proxy allocators
	allocator::make_delete_proxy(*getState()->proxyAllocators.flux, *getState()->mainAllocator);
	getState()->proxyAllocators.flux = nullptr;
	allocator::make_delete_proxy(*getState()->proxyAllocators.fuse, *getState()->mainAllocator);
	getState()->proxyAllocators.fuse = nullptr;
	allocator::make_delete_proxy(*getState()->proxyAllocators.asset, *getState()->mainAllocator);
	getState()->proxyAllocators.asset = nullptr;
	allocator::make_delete_proxy(*getState()->proxyAllocators.model, *getState()->mainAllocator);
	getState()->proxyAllocators.model = nullptr;
	allocator::make_delete_proxy(*getState()->proxyAllocators.entities, *getState()->mainAllocator);
	getState()->proxyAllocators.entities = nullptr;

	// Create a temporary pointer to the main allocator
	FreeListAllocator *tempAllocator = getState()->mainAllocator;

	// Delete the state object
	allocator::make_delete<State>(*tempAllocator, *getState());

	// Call the destructor of the main allocator
	tempAllocator->~FreeListAllocator();
	tempAllocator = nullptr;

	// Free the reserved memory
	free(arenaStart);
	arenaStart = nullptr;

	exit(errorCode);
}

flare::State *flare::getState() {

	return state;
}
