#include "flare/input.h"
#include "flare/engine.h"

/** @brief List of all possible keys and if they are pressed or not */
bool keys[25536] = {false};

/** @brief Mouse object
  @note Might not be neccesairy in the future */
flare::input::Mouse mouse;

bool flare::input::keyCheck(int key) {

	return keys[key];
}

void flare::input::keySet(int key, bool state) {

	keys[key] = state;
}

void flare::input::_keyCallback(GLFWwindow*, int key, int, int action, int) {

	if (action == GLFW_PRESS) {

		keys[key] = true;
	}

	if (action == GLFW_RELEASE) {

		keys[key] = false;
	}
}

flare::input::Mouse *flare::input::getMouse() {

	return &mouse;
}

void flare::input::update() {

	// mouse.delta = glm::vec2(0.0f, 0.0f);

	double xpos, ypos;
	glfwGetCursorPos(getState()->window, &xpos, &ypos);
	mouse.position = glm::vec2(xpos, ypos);
}
