#include "flare/flare.h"

struct Settings {

    // NOTE: These are the default engine settigs.
    const char *name = "flare engine";
    glm::ivec2 resolution = glm::ivec2(1280, 720);
    int swap = 0;
};

GLFWwindow *window = nullptr;

void flare::init() {

    // Initialize window
    // TODO: The settings need to be passed in from somewhere else
    Settings settings;

    log::d("The current resolution is %i by %i", settings.resolution.x, settings.resolution.y);

    if (!glfwInit()) {

	// TODO: Use logger
	log::e("Failed to initialize glfw!");
	exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // NOTE: First nullptr to glfwGetPrimaryMonitor for fullscreen
    window = glfwCreateWindow(settings.resolution.x, settings.resolution.y, settings.name, nullptr, nullptr);

    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    // TODO: Make this work properly
    glfwSetWindowPos(window, (mode->width-settings.resolution.x)/2 + 1366, (mode->height-settings.resolution.y)/2);

    glfwSetKeyCallback(window, input::keyCallback);

    glfwSwapInterval(settings.swap);

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {

	// TODO: Use logger
	log::e("Failed to initialize glew: %s", glewGetErrorString(glewStatus));
	exit(-1);
    }

    glGetError();

    // Initialize other systems
    render::init();

    log::d("Done initializing!");
}

bool flare::isRunning() {

    return !glfwWindowShouldClose(window);
}

void flare::update() {

    // NOTE: This is temporary, keybindings should definitely be rebindable
    if (input::keyCheck(GLFW_KEY_ESCAPE)) {

	glfwSetWindowShouldClose(window, GL_TRUE);
    }

    render::update();
    // tick::update();

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void flare::terminate() {

    glfwTerminate();
}
