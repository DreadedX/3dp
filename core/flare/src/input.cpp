#include "flare/flare.h"

bool keys[25536] = {false};

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

void flare::input::_mouseCallback(GLFWwindow*, double xpos, double ypos) {

    if (mouse._first) {
	mouse.position = glm::vec2(xpos, ypos);
	mouse._first = false;
    }

    if (keyCheck(GLFW_KEY_SPACE)) {

	mouse.yaw += (xpos - mouse.position.x) * getSettings()->mouseSensitivity;
	mouse.pitch += (mouse.position.y - ypos) * getSettings()->mouseSensitivity;
    }

    mouse.position = glm::vec2(xpos, ypos);
}

