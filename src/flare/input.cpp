#include "flare/flare.h"

bool keys[25536] = {false};

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

