#include "flare/flare.h"
#include <thread>
#include <chrono>

/** @brief Pointer the settings that are currently used by the engine
  @todo Add a way to set this externally */
flare::State *state = nullptr;

const size_t RESERVED_MEMORY = 1000 * 1000 * 11; /* 100 MB */

void *arenaStart;

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
	getState()->proxyAllocators.model = allocator::make_new_proxy(*getState()->mainAllocator);
	getState()->proxyAllocators.flux = allocator::make_new_proxy(*getState()->mainAllocator);
	getState()->proxyAllocators.fuse = allocator::make_new_proxy(*getState()->mainAllocator);

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

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// glfwWindowHint(GLFW_SAMPLES, 4);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// NOTE: First nullptr to glfwGetPrimaryMonitor for fullscreen
	getState()->window = glfwCreateWindow(settings->resolution.x, settings->resolution.y, settings->name, nullptr, nullptr);

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

	// Initialize other systems
	fuse::init(getState()->proxyAllocators.fuse);

	render::init();

	print::d("Engine initialized");
}

bool flare::isRunning() {

	return !glfwWindowShouldClose(getState()->window);
}

void flare::update() {

	// Calculate the deltaTime
	static float lastFrame = 0;
	float currentFrame = glfwGetTime();
	flare::getState()->render.deltaTime = (currentFrame - lastFrame);
	lastFrame = currentFrame;

	glfwPollEvents();
	flare::input::update();

	// NOTE: Debug keybindings
	{
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
	}

	// Run manager logic
	fuse::update();
	// Run renderer logic
	render::update();

	// This is for the debug interface
#ifndef NDEBUG
	ImGui_ImplGlfwGL3_NewFrame();
	{
		ImGui::Text("Delta time: %.2fms", getState()->render.deltaTime * 1000);
		ImGui::Text("Mouse position: %.2f, %.2f", input::getMouse()->position.x, input::getMouse()->position.y);
		ImGui::Text("Yaw/Pitch: %.2f, %.2f", getState()->render.camera.rotation.x, getState()->render.camera.rotation.y);
		ImGui::Text("Camera position: %.2f, %.2f, %.2f", getState()->render.camera.position.x, getState()->render.camera.position.y, getState()->render.camera.position.z);
		ImGui::Text("Memory usage (total): %lu bytes (%i%%)", getState()->mainAllocator->getUsedMemory(), (int)(getState()->mainAllocator->getUsedMemory()*100/getState()->mainAllocator->getSize()));
		ImGui::Text("Memory usage (model): %lu bytes (%i%%)", getState()->proxyAllocators.model->getUsedMemory(), (int)(getState()->proxyAllocators.model->getUsedMemory()*100/getState()->proxyAllocators.model->getSize()));
		ImGui::Text("Memory usage (flux): %lu bytes (%i%%)", getState()->proxyAllocators.flux->getUsedMemory(), (int)(getState()->proxyAllocators.flux->getUsedMemory()*100/getState()->proxyAllocators.flux->getSize()));
		ImGui::Text("Memory usage (fuse): %lu bytes (%i%%)", getState()->proxyAllocators.fuse->getUsedMemory(), (int)(getState()->proxyAllocators.fuse->getUsedMemory()*100/getState()->proxyAllocators.fuse->getSize()));
		debug::entityTree();
	}
	ImGui::Render();
#endif

	glfwSwapBuffers(getState()->window);
}

void flare::terminate(int errorCode) {

	// Kill and remove all remaining entities
	fuse::killAll();
	fuse::update();

	glfwTerminate();

	// Close all open asset files
	asset::close();

	print::d("The engine is now exiting");

	// Remove all proxy allocators
	allocator::make_delete_proxy(*getState()->proxyAllocators.model, *getState()->mainAllocator);
	getState()->proxyAllocators.model = nullptr;
	allocator::make_delete_proxy(*getState()->proxyAllocators.flux, *getState()->mainAllocator);
	getState()->proxyAllocators.flux = nullptr;
	allocator::make_delete_proxy(*getState()->proxyAllocators.fuse, *getState()->mainAllocator);
	getState()->proxyAllocators.fuse = nullptr;

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
