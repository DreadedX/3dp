#include "flare/flare.h"
#include <thread>
#include <chrono>

GLFWwindow *window = nullptr;
flare::Settings *settings = new flare::Settings();

double delta = 1/60;

void flare::init() {

    // Initialize window
    // TODO: The settings need to be passed in from somewhere else
    // TODO: Part of this should propably move into the renderer

    print::d("The current resolution is %i by %i", settings->resolution.x, settings->resolution.y);

    if (!glfwInit()) {

	// TODO: Use logger
	print::e("Failed to initialize glfw!");
	exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // 16x MSAA
    glfwWindowHint(GLFW_SAMPLES, 16);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // NOTE: First nullptr to glfwGetPrimaryMonitor for fullscreen
    window = glfwCreateWindow(settings->resolution.x, settings->resolution.y, settings->name, nullptr, nullptr);

    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    // TODO: Make this work properly
    glfwSetWindowPos(window, (mode->width - settings->resolution.x) / 2 + 1366, (mode->height - settings->resolution.y) / 2);

    glfwSetKeyCallback(window, input::_keyCallback);
    glfwSetCursorPosCallback(window, input::_mouseCallback);

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {

	// TODO: Use logger
	print::e("Failed to initialize glew: %s", glewGetErrorString(glewStatus));
	exit(-1);
    }

    #ifndef NDEBUG
	// TODO: Install own callbacks
	// TODO: This sets things like swap, that needs to be disabled
	ImGui_ImplGlfwGL3_Init(window, false);
    #endif

    glfwSwapInterval(settings->swap);

    glGetError();

    // Load all asset files
    flux::load();

    // Initialize other systems
    render::init();

    print::d("Done initializing!");
}

bool flare::isRunning() {

    return !glfwWindowShouldClose(window);
}

void flare::update() {
    
    // Calculate the deltaTime
    static float lastFrame = 0;
    float currentFrame = glfwGetTime();
    flare::render::getState()->deltaTime = (currentFrame - lastFrame);
    lastFrame = currentFrame;

    glfwPollEvents();

    // NOTE: Debug keybindings
    {
	if (input::keyCheck(GLFW_KEY_ESCAPE)) {

	    glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (input::keyCheck(GLFW_KEY_F5)) {

	    // NOTE: This only works on linux and for testing only
	    // TODO: Build in fluxuate, needs to move to the editor in the future
	    system("cd ../../CMake/default && make assets");

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

    // Run entity render
    fuse::draw();
    
    // This is for the debug interface
    #ifndef NDEBUG
	ImGui_ImplGlfwGL3_NewFrame();
	{
	    ImGui::Text("Delta time: %.2fms", render::getState()->deltaTime * 1000);
	    ImGui::Text("Mouse position: %.2f, %.2f", flare::input::getMouse()->position.x, input::getMouse()->position.y);
	    ImGui::Text("Yaw/Pitch: %.2f, %.2f", flare::input::getMouse()->yaw, input::getMouse()->pitch);
	    debug::entityTree();
	}
	ImGui::Render();
    #endif

    glfwSwapBuffers(window);
}

void flare::terminate(int errorCode) {

    delete settings;

    // Kill and remove all remaining entities
    fuse::killAll();
    fuse::update();

    glfwTerminate();

    // Close all open asset files
    asset::close();

    print::d("The engine is now exiting");

    exit(errorCode);
}

flare::Settings *flare::getSettings() {

    return settings;
}
